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


static uint32_t calculate_char_array_fitness(const char *ca)
{
	uint32_t i, fitness = 0;

	for (i = 0; i < strlen(target_text); i++) {
		if (ca[i] == target_text[i]) {
			fitness += 1;
		}
	}
	return fitness;
}

uint32_t calculate_chromosome_fitness(struct chromosome *chromosome)
{
	return calculate_char_array_fitness(chromosome->chromosome);
}

static void print_current_fittest(struct population_pool *population_pool)
{
	/* print the current fittest, search the whole pool */
	(void) population_pool;
}


/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
