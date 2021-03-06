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

#include <assert.h>

#include "cfga.h"

extern const char target_text[];

static int compare_uint32_keys(uint32_t a, uint32_t b)
{
	if (a < b)
		return -1;
	else if (a > b)
		return 1;
	else
		return 0;
}

struct population_pool* alloc_population_pool(void)
{
	struct population_pool *ret;
	ret = xalloc(sizeof(struct population_pool));

	ret->root = RB_ROOT;
	ret->cmp = compare_uint32_keys;
	ret->count = 0;

	return ret;
}

void free_population_pool(struct population_pool *p)
{
	free(p);
}


int generate_alpha_character(void)
{
	return (((int)rand()) % (122 - 97 + 1)) + 97;
}


int breed_initial_population(struct population_pool *population_pool,
		uint32_t population_size)
{
	uint32_t i; int ret = 0;

	/* generate population_size random populations */
	for (i = 0; i < population_size; i++) {

		uint32_t j;
		struct chromosome *chromosome;

		chromosome = xalloc(sizeof(struct chromosome));

		chromosome->fitness = 0;
		chromosome->chromosome = xalloc(strlen(target_text) + 1);

		for (j = 0; j < strlen(target_text); j++) {
			chromosome->chromosome[j] = generate_alpha_character();
		}

		add_chromosome_into_popolation(population_pool, chromosome);
	}

	return ret;
}

int remove_worst_from_population(struct population_pool *population_pool)
{
	struct population_group *node;
	struct chromosome *iterator;
	int random_drop;

	for (node = rb_last((struct rb_root *)population_pool); node; node = rb_next((struct rb_node *)node)) {

		struct population_group *population_group = rb_entry(node, struct population_group, node);

		if (population_group->chromosome_quantity <= 0) {
			die("Programmed error - more then as 0 chromosomes expected");
		}

		random_drop = rand() % population_group->chromosome_quantity;

		list_for_each_entry(iterator, &population_group->list, list) {
			if (!random_drop--)
				goto out;
		}

	}
out:
	remove_chromosome_from_population(iterator, population_pool);
}


int remove_chromosome_from_population(
		struct chromosome *chromosome, struct population_pool *population_pool)
{
	uint32_t fitness; int ret;
	struct population_group *population_group = NULL;
	struct chromosome *iterator;

	/* calculate fitness of chromosome */
	fitness = calculate_chromosome_fitness(chromosome);
	chromosome->fitness = fitness;

	/* try to fetch population_group container with fitness fitness */
	ret = lhi_get_rbtree(population_pool, fitness, &population_group);
	if (ret == -1)
		return NULL;

	/* iterate over population group list */
	list_for_each_entry(iterator, &population_group->list, list) {
		uint32_t i; int found_match = 1;

		for (i = 0; i < strlen(target_text); i++) {
			if (iterator->chromosome[i] != target_text[i])
				found_match = 0;
		}
		if (found_match) {
			list_del(&iterator->list);
			free(iterator->chromosome);
			free(iterator);
		}
	}

	if (population_group->chromosome_quantity == 1) {
		/* this was the last element in the list
		 * remove the population_group also! */
		ret = lhi_remove_rbtree(population_pool, fitness, &population_group);
		if (ret == -1) {
			fprintf(stderr, "Internal Error: can't remove population group\n");
			exit(1);
		}
		free_population_group(population_group);
	} else {
		population_group->chromosome_quantity--;
	}

	population_pool->count--;

	return SUCCESS;
}

struct population_group* alloc_population_group(void)
{
	struct population_group *population_group;

	population_group = xalloc(sizeof(struct population_group));
	return population_group;
}

void free_population_group(struct population_group *p)
{
	free(p);
}

int add_chromosome_into_popolation_group(struct population_group *population_group,
		struct chromosome *chromosome)
{
	struct chromosome *iterator;

	if (population_group->fitness != chromosome->fitness) {
		fprintf(stderr, "Internal error: fitness differs!\n");
		exit(1);
	}

	/* add to list */
	list_add_tail(&(chromosome->list), &(population_group->list));

	population_group->chromosome_quantity++;
}

/**
 * Hidden interface, only called from add_chromosome_into_popolation()
 */
static int add_popolation_group_into_population_pool(struct population_pool *population_pool,
		struct population_group *population_group)
{
	int ret;

	ret = lhi_insert_rbtree(population_pool, population_group->fitness, population_group);
	if (ret < 0) {
		fprintf(stderr, "Can't insert population_group into population_pool\n");
		return -1;
	}

	return 0;
}

int add_chromosome_into_popolation(struct population_pool *population_pool,
		struct chromosome *chromosome)
{
		int ret; uint32_t fitness;
		struct population_group *population_group = NULL;

		/* calculate fitness for chromosome */
		fitness = calculate_chromosome_fitness(chromosome);
		chromosome->fitness = fitness;

		/* try to fetch population_group container with fitness fitness */
		ret = lhi_get_rbtree(population_pool, fitness, &population_group);
		if (ret == -1) {
			/* new population group required */
			int iret;

			population_group = alloc_population_group();
			population_group->fitness = fitness;
			population_group->chromosome_quantity = 0;

			INIT_LIST_HEAD(&(population_group->list));

			iret = add_chromosome_into_popolation_group(population_group, chromosome);
			if (iret < 0) {
				fprintf(stderr, "Can't insert chromosome into population group\n");
			}

			iret = add_popolation_group_into_population_pool(population_pool,
					population_group);
			if (iret < 0) {
				fprintf(stderr, "Can't insert population group into population pool\n");
			}
		} else {
			/* population group with fitness already in
			 * population_pool present, so we add only the chromosome
			 * into the proper group */
			int iret;

			iret = add_chromosome_into_popolation_group(population_group, chromosome);
			if (iret < 0) {
				fprintf(stderr, "Can't insert chromosome into population group\n");
			}
		}

		population_pool->count++;

		return 0;
}

void print_population_fitness(struct population_pool *population_pool)
{
	struct population_group *node;
	for (node = rb_first(population_pool); node; node = rb_next(node)) {
		struct population_group *population_group =
			rb_entry(node, struct population_group, node);
		fprintf(stdout, "fitness: %u, quantity: %u\n",
				population_group->fitness, population_group->chromosome_quantity);
	}
	fprintf(stdout, " [cumulative chromosomes: %u]\n", population_pool->count);
}



/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
