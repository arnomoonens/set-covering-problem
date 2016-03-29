//
//  solution.h
//  SCP
//
// Code or initializing, manipulating and freeing a solution
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef solution_h
#define solution_h

#include <stdio.h>
#include <string.h>

#include "instance.h"

/** Holds all solution information **/
struct Solution {
    int *x;           /* x[i] 0,1 if column (= set) i is selected */
    int *y;           /* y[i] 0,1 if row (=element) i covered by the actual solution */
    /** Note: Use incremental updates for the solution **/
    int fx;           /* sum of the cost of the columns selected in the solution (can be partial) */
    
    /** Dynamic variables **/
    /** Note: use dynamic variables to make easier the construction and modification of solutions.  **/
    /**       these are just examples of useful variables.                                          **/
    /**       these variables need to be updated every time a column is added to a partial solution **/
    /**       or when a complete solution is modified*/
    int used_sets;
    int **col_cover;   /* col_colver[i] selected columns that cover row i */
    int *ncol_cover;   /* number of selected columns that cover row i */
};

struct Solution *initialize(struct Instance *instance);
struct Solution *copy_solution(struct Instance *instance, struct Solution *source);
void remove_set(struct Instance *instance, struct Solution *sol, int set);
int uncovered_elements(struct Instance *instance, struct Solution *sol);
int added_elements(struct Instance *instance, struct Solution *sol, int set);
int max_cost(struct Instance *instance, struct Solution *sol);
void free_solution(struct Solution *sol);

#endif /* solution_h */
