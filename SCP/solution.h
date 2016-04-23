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
typedef struct Solution solution;
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

solution *initialize(instance *inst);
solution *copy_solution(instance *inst, solution *source);
void add_set(instance *inst, solution *sol, int set);
void remove_set(instance *inst, solution *sol, int set);
int uncovered_elements(instance *inst, solution *sol);
int added_elements(instance *inst, solution *sol, int set);
int find_max_weight_set(instance *inst, solution *sol, int ctr);
void redundancy_elimination(instance *inst, solution *sol);
int max_cost(instance *inst, solution *sol);
void column_inclusion(instance *inst,  solution *sol);
void free_solution(instance *inst, solution *sol);

#endif /* solution_h */
