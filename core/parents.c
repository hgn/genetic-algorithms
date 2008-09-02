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

extern const char target_text[];


static int select_random_n_parents(
		struct population_pool *population_pool, uint32_t n, struct chromosome **ca)
{
	int ret = 0;

	return ret;
}

/**
 * We know that the particular population group have */
static int grap_n_randomly(struct population_group *node, uint32_t ptr,
		uint32_t still_required, struct chromosome **c)
{
	int randret;
	struct chromosome *iterator;

	while (1) {

		int already_in_pool = 0;

		randret = rand() % node->chromosome_quantity;
			fprintf(stderr, "rand: %d\n", randret);

		list_for_each_entry(iterator, &node->list, list) {



			if (randret-- <= 0) {
				fprintf(stderr, "XXX 2\n");
				/* we didn't found the string in the local pool, so copy */
				memcpy(&c[ptr]->chromosome, &iterator->chromosome, strlen(iterator->chromosome) + 1);
				ptr++;
				still_required--;

				if (still_required <= 0) {
					return 0;
				}

			}
		}
	}
}

/**
 * This strategy identfies the highest fitness pool and randomly graps
 * n elements from this pool. If the pool isn't sufficient (e.g. because
 * one n - 1 elements within this pool) it will grep the remaining
 * chromosomes from the lower fitness population group
 */
static int select_best_n_parents(
		struct population_pool *population_pool, uint32_t n, struct chromosome ***ca)
{
	int ret = 0;
	uint32_t i, ptr;

	struct chromosome **ret_chromo;

	/* allocate memory for return value */
	ret_chromo = xalloc(sizeof(struct chromosome *) * n);
	for (i = 0; i <= n; i++) {
		ret_chromo[i] = xalloc(sizeof(struct chromosome));
		ret_chromo[i]->chromosome = xalloc(strlen(target_text) + 1);
	}

	uint32_t still_required = n;
	ptr = 0;


		struct chromosome *iterator;

		struct population_group *node;


		for (node = rb_first((void *)population_pool); node; node = rb_next((void *)node)) {

			struct population_group *population_group = rb_entry(node, struct population_group, node);

			if (population_group->chromosome_quantity >= still_required) {
				/* fine, all chromosomes are in one vertice, grap
				 * randomly */
				grap_n_randomly(population_group, ptr, still_required, ret_chromo);

				fprintf(stderr, "new parents p1: %s p2: %s\n", ret_chromo[0]->chromosome, ret_chromo[1]->chromosome);

				/* we got all we want -> break out */
				goto out;

			} else {
				/* copy all, book-keeping and next round */
				list_for_each_entry(iterator, &population_group->list, list) {
					memcpy(&ret_chromo[ptr]->chromosome, &iterator->chromosome, strlen(iterator->chromosome) + 1);
					ptr++;
					still_required--;
					if (still_required == 0) {
						fprintf(stderr, "new parents p1: %s p2: %s\n", ret_chromo[0]->chromosome, ret_chromo[1]->chromosome);
						goto out;
					}
				}
			}
		}
out:
	*ca = ret_chromo;
	return ret;
}

void free_best_n_parents(uint32_t n, struct chromosome **ca)
{
	uint32_t i;

	for (i = 0; i < n; i++) {
		free(ca[i]->chromosome);
		free(ca[i]);
	}
	free(ca);
}

int select_n_parents(struct population_pool *population_pool, uint32_t n,
		enum select_strategy ss, struct chromosome **ret)
{
	switch (ss) {
		case SELECT_RANDOM:
			return select_random_n_parents(population_pool, n, ret);
			break;
		case SELECT_BEST:
			return select_best_n_parents(population_pool, n, ret);
			break;
		default:
			die("Programmed error in switch/case statement\n");
			break;
	};
}



/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
