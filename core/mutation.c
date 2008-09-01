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

#define	PERCENTAL_MUTATION_MAX 10

extern const char target_text[];

void init_mutation(char *offspring)
{
	uint16_t j;
	int percental_mutation;

	percental_mutation = rand() % PERCENTAL_MUTATION_MAX;

	/* FIXME: implement this ;) */
	for (j = 0; j < strlen(target_text); j++) {

		if (rand() % strlen(target_text) == 0) {
			if (rand() % 1 == 0) {
				offspring[j] += 1;
				if (offspring[j] > 122) {
					offspring[j] = 122;
				}
			} else {
				offspring[j] -= 1;
				if (offspring[j] < 97) {
					offspring[j] = 97;
				}
			}
		}
	}

	if (VERBOSE_LEVEL >= 1)
		fprintf(stdout, "mutated offspring: %s\n", offspring);
}


/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
