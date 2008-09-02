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

int do_crossover_and_mutation(struct chromosome **parents,
		uint32_t no_parents, struct chromosome **offspring)
{
	int i, ret = SUCCESS;

	struct chromosome *rc = xalloc(sizeof(struct chromosome));
	rc->chromosome = xalloc(strlen(target_text) + 1);

	/* first of all crossover */

	for (i = 0; i < strlen(target_text); i++) {
		if (rand() % 2 == 0) {
			/* took p1 as descent */
			rc->chromosome[i] = parents[0]->chromosome[i];
		} else {
			/* took p2 as descent */
			rc->chromosome[i] =  parents[1]->chromosome[i];
		}
	}

	/* mutation */
	init_mutation(rc->chromosome);

	*offspring = rc;
	return ret;
}



/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
