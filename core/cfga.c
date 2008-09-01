/*
** Copyright (C) 2008 - Hagen Paul Pfeifer <hagen@jauu.net>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "cfga.h"


const char target_text[] = "xxxxxxxx";

#define	RANDOMFILE "/dev/urandom"

struct entity {
	char *chromosome;
	uint32_t fitness;
};


void die(const char const *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void *xalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (!ptr)
		die("out of memory");

	memset(ptr, 0, size);

	return ptr;
}

static void init_random_seed(void)
{
	uint32_t rand_seed, new_seed;
	int ufd = open(RANDOMFILE, O_RDONLY);

	if (ufd == -1) {
		new_seed = ((((int)time(NULL)) & ((1 << 30) - 1)) + getpid());
	} else {
		if (sizeof(rand_seed) != read(ufd, &rand_seed, sizeof(rand_seed))) {
			fprintf(stderr, "read(2) error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		close(ufd);
		new_seed = rand_seed;
	}

	srand(new_seed);
}

static void gen_initial_population(struct entity ***eep)
{
	uint16_t i, j;

	struct entity **entities = xalloc(sizeof(struct entity *) * POPULATION_SIZE);

	for (j = 0; j < POPULATION_SIZE; j++) {

		entities[j] = xalloc(sizeof(struct entity));

		/* allocate string mem */
		entities[j]->chromosome = xalloc(strlen(target_text) + 1);

		for (i = 0; i < strlen(target_text); i++)
			entities[j]->chromosome[i] = (((int)rand()) % (122 - 97 + 1)) + 97;

		if (VERBOSE_LEVEL >= 2)
			fprintf(stdout, "new chromosome %u: %s\n", j, entities[j]->chromosome);
	}

	*eep = entities;
}

static void replace_worst_with_offspring(struct entity **entities,
		char *offspring, uint16_t fitness)
{
	struct entity *unfittest = NULL;
	uint16_t un_fittest_val = -1, i;

	/* search unfittest */
	for (i = 0; i < POPULATION_SIZE; i++) {

		struct entity *entity = entities[i];

		if (entity->fitness < un_fittest_val) { // found one
			un_fittest_val = entity->fitness;
			unfittest = entity;
		}
	}

	/* unfittest is a pointer to the unfittest entity */

	if (un_fittest_val > fitness) {
		/* nothing to do, because we are unfitter */
		return;
	}

	/* and replace */
	memcpy(unfittest->chromosome, offspring, strlen(target_text));
	unfittest->fitness = fitness;

}

static void eval_each_individual(struct entity **entities)
{
	uint16_t i, j;

	for (i = 0; i < POPULATION_SIZE; i++) {

		struct entity *entity = entities[i];

		/* reset fitness */
		entity->fitness = 0;

		/* loop over chromosome */
		for (j = 0; j < strlen(target_text); j++) {
			if (entity->chromosome[j] == target_text[j]) {
				entity->fitness += 1;
			}
		}
	}
}

static uint16_t calc_offspring_fitness(char *offspring)
{
	uint16_t j, fitness = 0;

	/* loop over chromosome */
	for (j = 0; j < strlen(target_text); j++) {
		if (offspring[j] == target_text[j]) {
			fitness += 1;
		}
	}

	return fitness;
}

static void print_current_fittest(struct entity **entities)
{
	uint16_t i, current_fittest = 0;
	char *fittest;

	/* catch all in the case of equals of all chromosomes */
	fittest = entities[0]->chromosome;

	for (i = 0; i < POPULATION_SIZE; i++) {
		struct entity *entity = entities[i];

		if (entity->fitness > current_fittest) {
			current_fittest = entity->fitness;
			fittest = entity->chromosome;
		}
	}

	fprintf(stderr, "current fittest: %s (%u)\n", fittest, current_fittest);
}

#define	POS_FIRST  0
#define	POS_SECOND 1
#define	POS_VALUE  0
#define	POS_RANK   1

static void select_best_first_two(struct entity **entities, char **top_chromosomes)
{
	uint8_t fittest_value = 0;
	uint16_t i, j, last_parent_marker;
	unsigned int fittest_count = 0;
	char **chromosome_fittest_ring;

	top_chromosomes[0] = xalloc(strlen(target_text) + 1);
	top_chromosomes[1] = xalloc(strlen(target_text) + 1);

	/* search fittest rank value */
	for (i = 0; i < POPULATION_SIZE; i++) {
		if (entities[i]->fitness > fittest_value)
			fittest_value = entities[i]->fitness;
	}

	/* count how many fittest with value fittest_value we had */
	fittest_count = 0;
	for (i = 0; i < POPULATION_SIZE; i++) {
		if (entities[i]->fitness == fittest_value)
			fittest_count++;
	}

	if (VERBOSE_LEVEL >= 2)
		fprintf(stderr, "found %u fittest\n", fittest_count);

	chromosome_fittest_ring = xalloc(fittest_count * sizeof(char *));

	/* replace this and the previous paragraph via realloc(3) */
	for (i = 0, j = 0; i < POPULATION_SIZE; i++) {
		if (entities[i]->fitness == fittest_value) {
			chromosome_fittest_ring[j] = xalloc(strlen(target_text) + 1);
			memcpy(&chromosome_fittest_ring[j], entities[i]->chromosome,
					strlen(target_text));
			j++;
		}
	}

	/* FIXME:
	 * we have a problem here: the current algorithm assume
	 * that the fittest group had always more then one member. This is not
	 * true at all. Therefore if fittest_count == 1 we need a parent from
	 * a lower rated class -- HGN */
	i = 0; last_parent_marker = 0;
	while (1) {

		int rval = rand() % fittest_count;
		if (i == 1 && rval == last_parent_marker)
			continue; /* we want unique parent - no dubplicates */

		memcpy(top_chromosomes[i],
				&chromosome_fittest_ring[rval], strlen(target_text) + 1);
		if (i == 1) /* we got what we wanted */
			break;

		i++;
		last_parent_marker = rval;
	}
}

/* in opposite from select_best_first_two(), this method groups the fittest
 * ones (with the same fitness value) into one group and randomly select two
 * offsprings */
void select_best_two_from_pool(struct entity **entities, char **top_chromosomes)
{
	uint16_t i, current_fittest[2][2] = { {0U, 0U}, {0U, 0U} };
	char *fittest[2];

	fittest[POS_FIRST] = entities[0]->chromosome;
	fittest[POS_SECOND] = entities[1]->chromosome;

	top_chromosomes[0] = xalloc(strlen(target_text) + 1);
	top_chromosomes[1] = xalloc(strlen(target_text) + 1);


	/* search fittest */
	for (i = 0; i < POPULATION_SIZE; i++) {
		struct entity *entity = entities[i];

		if (entity->fitness > current_fittest[POS_FIRST][POS_VALUE]) {

			current_fittest[POS_FIRST][POS_VALUE]  = entity->fitness;
			current_fittest[POS_FIRST][POS_RANK] = i;

			fittest[POS_FIRST] = entity->chromosome;

			if (VERBOSE_LEVEL >= 2)
				fprintf(stderr, "found new fittest(1): %s - values: %u\n",
						entity->chromosome, entity->fitness);
		}
	}

	/* search fittest, fittest */
	for (i = 0; i < POPULATION_SIZE; i++) {

		/* prevent that we cross the some individuals */
		if (i == current_fittest[POS_FIRST][POS_RANK])
			continue;

		struct entity *entity = entities[i];

		if (entity->fitness > current_fittest[POS_SECOND][POS_VALUE]) {
			current_fittest[POS_SECOND][POS_VALUE]  = entity->fitness;
			current_fittest[POS_SECOND][POS_RANK] = i;

			fittest[POS_SECOND] = entity->chromosome;

			if (VERBOSE_LEVEL >= 2)
				fprintf(stderr, "found new fittest(2): %s - values: %u\n",
						entity->chromosome, entity->fitness);
		}
	}

	memcpy(top_chromosomes[0],  fittest[POS_FIRST], strlen(target_text) + 1);
	memcpy(top_chromosomes[1],  fittest[POS_SECOND], strlen(target_text) + 1);
}

static void free_best_two(char **top_chromosomes)
{
	free(top_chromosomes[0]);
	free(top_chromosomes[1]);
}

static char *crossover_single_point(char *p1, char *p2)
{
	uint16_t border;
	char *offspring;
	offspring = xalloc(strlen(target_text) + 1);

	/* get some random border between 0 and strlen(target_text) */
	border = rand() % strlen(target_text);

	memcpy(offspring, p1, border);
	memcpy(&offspring[border], &p2[border], strlen(target_text) - border);

	if (VERBOSE_LEVEL >= 1)
		fprintf(stderr, "new offspring: %s (p1: %s, p2: %s, border %u)\n",
				offspring, p1, p2, border);

	return offspring;
}


static char *crossover_two_point(char *p1, char *p2)
{
	char *offspring;

	(void) p1; (void) p2;

	offspring = xalloc(strlen(target_text) + 1);
	return offspring;
}


static char *crossover_uniform(char *p1, char *p2)
{
	uint16_t i;
	char *offspring;

	offspring = xalloc(strlen(target_text) + 1);

	for (i = 0; i < strlen(target_text); i++) {

		if (rand() % 2 == 0) {
			/* took p1 as descent */
			offspring[i] = p1[i];
		} else {
			/* took p2 as descent */
			offspring[i] = p2[i];
		}
	}

	fprintf(stderr, "\t\t\tp1: %s p2: %s - mutant: %s\n", p1, p2, offspring);

	return offspring;
}


static char *crossover_arithmetic(char *p1, char *p2)
{
	char *offspring;

	(void) p1; (void) p2;

	offspring = xalloc(strlen(target_text) + 1);
	return offspring;
}


static char *init_crossover(char *p1, char *p2)
{

	/* see http://www.obitko.com/tutorials/genetic-algorithms/crossover-mutation.php
	 * for the definition of the crossover modes */

	switch (BINARY_ENCODING_CROSSOVER_MODE) {
		case SINGLE_POINT_CROSSOVER:
			return crossover_single_point(p1, p2);
			break;
		case TWO_POINT_CROSSOVER:
			return crossover_two_point(p1, p2);
			break;
		case UNIFORM_CROSSOVER:
			return crossover_uniform(p1, p2);
			break;
		case ARITHMETIC_CROSSOVER:
			return crossover_arithmetic(p1, p2);
			break;
		default:
			die("Programmed error in switch/case statement");
			break;
	}
}


static void close_all_open_fds(void)
{
	int i;

	for (i = 0; i < 128; i++)
		close(i);
}

int main(void)
{

	struct population_pool *population_pool;

	init_random_seed();

	population_pool = alloc_population_pool();
	breed_initial_population(population_pool, POPULATION_SIZE);

	return 0;
}


int mainold(void)
{
	struct entity **entities;
	char *top_chromosomes[2];

	if (VERBOSE_LEVEL <= 0)
		close_all_open_fds();

	init_random_seed();

	/* Choose initial population */
	gen_initial_population(&entities);

	/* Evaluate the fitness of each individual in the population */
	eval_each_individual(entities);
	print_current_fittest(entities);

	/* Repeat until termination */
	while (1) {

		/* Select best-ranking individuals to reproduce */
		select_best_first_two(entities, top_chromosomes);
		if (VERBOSE_LEVEL >= 1)
			fprintf(stderr, "1:  %s  2: %s \n", top_chromosomes[0], top_chromosomes[1]);

		/* Breed new generation through crossover and mutation (genetic
		 * operations) and give birth to offspring
		 */
		char *offspring = init_crossover(top_chromosomes[0], top_chromosomes[1]);

		if (!strcmp(offspring, target_text)) {
			fprintf(stderr, "found target\n");
			exit(0);
		}

		init_mutation(offspring);

		/* evaluate the individual fitnesses of the offspring */
		uint16_t fitness = calc_offspring_fitness(offspring);
		fprintf(stdout, "offspring fitness: %u\n", fitness);

		replace_worst_with_offspring(entities, offspring, fitness);

		free_best_two(top_chromosomes);
		free(offspring);
	}


	return 0;
}




/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
