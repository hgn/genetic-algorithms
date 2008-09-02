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
 * This strategy identfies the highest fitness pool and randomly graps
 * n elements from this pool. If the pool isn't sufficient (e.g. because
 * one n - 1 elements within this pool) it will grep the remaining
 * chromosomes from the lower fitness population group
 */
static int select_best_n_parents(
		struct population_pool *population_pool, uint32_t n, struct chromosome **ca)
{
	int ret = 0;
	uint32_t i;

	struct chromosome **c = xalloc(sizeof(struct chromosome *) * n);

	for (i = 0; i < n; i++) {
		c[i] = xalloc(sizeof(struct chromosome));
		c[i]->chromosome = xalloc(strlen(target_text) + 1);
	}

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
