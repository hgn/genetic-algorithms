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


char *init_crossover(char *p1, char *p2)
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



/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
