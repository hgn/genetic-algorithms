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

#ifndef CFGA_H_
#define CFGA_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#include "rbtree.h"
#include "list.h"

#define	SINGLE_POINT_CROSSOVER 1
#define	TWO_POINT_CROSSOVER    2
#define	UNIFORM_CROSSOVER      3
#define	ARITHMETIC_CROSSOVER   4
#define	BINARY_ENCODING_CROSSOVER_MODE UNIFORM_CROSSOVER

#define	POPULATION_SIZE 200

#define	VERBOSE_LEVEL 2

struct rb_root
{
	struct rb_node *rb_node;
};

enum select_strategy {
	SELECT_RANDOM = 1,
	SELECT_BEST
};


struct chromosome {
	struct list_head list;
	uint32_t fitness;
	char *chromosome;
};

/* a particular population with the same fitness value -
 * a population group */
struct population_group {
	uint32_t fitness;
	struct rb_node node;
	struct list_head list;
	uint32_t chromosome_quantity;
};



/* this is the data structure where *all* population_groups are stored */
struct population_pool {
	struct rb_root root;
	int (*cmp)(uint32_t, uint32_t);
	uint32_t count;
};

#define	SUCCESS 0
#define	FAILURE -1

void init_mutation(char *);
void *xalloc(size_t);
void die(const char const *);
struct population_pool* alloc_population_pool(void);
void free_population_pool(struct population_pool *);

int breed_initial_population(struct population_pool *, uint32_t);

int add_chromosome_into_popolation(struct population_pool *, struct chromosome *);
void free_population_group(struct population_group *);
uint32_t calculate_chromosome_fitness(struct chromosome *);
int lhi_get_rbtree(struct population_pool *, const uint32_t, struct population_group **);
int lhi_remove_rbtree(struct population_pool *, const uint32_t, struct population_group **);
int lhi_insert_rbtree(struct population_pool *, const uint32_t, struct population_group *);
void print_population_fitness(struct population_pool *);

int select_n_parents(struct population_pool *, uint32_t, enum select_strategy, struct chromosome **);

int do_crossover_and_mutation(struct chromosome **, uint32_t, struct chromosome **);

struct rb_node *rb_first(struct rb_root *);
struct rb_node *rb_last(struct rb_root *);
struct rb_node *rb_next(struct rb_node *);
struct rb_node *rb_prev(struct rb_node *);

#endif /* CFGA_H_ */


/* vi: set tw=78 sw=4 ts=4 sts=4 ff=unix noet: */
