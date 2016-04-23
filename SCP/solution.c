//
//  solution.c
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "solution.h"

/*** Initialize a solution and all its variables **/
solution *initialize(instance *inst) {
    solution *sol = mymalloc(sizeof(solution));
    int i, j;
    sol->x = (int *) mymalloc(inst->n*sizeof(int));
    for (i = 0; i < inst->n; i++) {sol->x[i] = 0;}
    sol->y = (int *) mymalloc(inst->m*sizeof(int));
    for (i = 0; i < inst->m; i++) {sol->y[i] = 0;}

    sol->col_cover = (int **) mymalloc(inst->m*sizeof(int *));
    for (i=0; i<inst->m; i++) {
        sol->col_cover[i] = (int *) mymalloc(inst->ncol[i]*sizeof(int));
        for (j = 0; j < inst->ncol[i]; j++) {
            sol->col_cover[i][j] = -1;
        }
    }

    sol->ncol_cover = (int *) mymalloc(inst->m*sizeof(int));
    for (i = 0; i<inst->m; i++) {sol->ncol_cover[i] = 0;}
    sol->fx = 0;
    sol->used_sets = 0;
    return sol;
}


/** Copy a solution to a newly initialized one **/
solution *copy_solution(instance *inst, solution *source) {
    int i;
    solution *new_sol = initialize(inst);
    new_sol->used_sets = source->used_sets;
    new_sol->fx = source->fx;

    memcpy(new_sol->x, source->x, inst->n * sizeof(int));
    memcpy(new_sol->y, source->y, inst->m * sizeof(int));

    for (i=0; i<inst->m; i++) {
        memcpy(new_sol->col_cover[i], source->col_cover[i], source->ncol_cover[i] * sizeof(int));
    }
    memcpy(new_sol->ncol_cover, source->ncol_cover, inst->m * sizeof(int));

    return new_sol;
}

// Add a set to the current solution
void add_set(instance *inst, solution *sol, int set) {
    int i, element;
    for (i = 0; i < inst->nrow[set]; i++) { //Say that we cover each element that the chosen set covers
        element = inst->row[set][i];
        sol->y[element] = 1;
        sol->col_cover[element][sol->ncol_cover[element]] = set;
        sol->ncol_cover[element]++;
    }
    sol->used_sets++;
    sol->x[set] = 1;
    sol->fx += inst->cost[set];
    return;
}


/** Remove a set from a solution **/
void remove_set(instance *inst, solution *sol, int set) {
    int i, j, k;
    sol->used_sets--;
    sol->x[set] = 0;
    sol->fx -= inst->cost[set];
    for (i = 0; i < inst->m; i++) { //Check for each element...
        for (j = 0; j < sol->ncol_cover[i]; j++) {
            if(sol->col_cover[i][j] == set) { //If the set to be removed covers the element...
                for (k = j+1; k < sol->ncol_cover[i]; k++) { //then shift set indices to the left starting from j+1
                    sol->col_cover[i][k-1] = sol->col_cover[i][k];
                }
                sol->ncol_cover[i]--;
                if (!sol->ncol_cover[i]) { //No included sets cover element i anymore
                    sol->y[i] = 0;
                }
                break;
            }
        }
    }
    return;
}

/** Check if some elements aren't covered by a set yet in the current solution **/
int uncovered_elements(instance *inst, solution *sol) {
    int i;
    for (i = 0; i < inst->m; i++)
        if(!sol->y[i]) return 1;
    return 0;
}

/** Returns how many yet uncovered elements the set would cover **/
int added_elements(instance *inst, solution *sol, int set) {
    int count = 0;
    int i;
    for (i = 0; i < inst->nrow[set]; i++)
        if (!sol->y[inst->row[set][i]]) count++;
    return count;
}

/** Find the ctr'th highest cost set of an instance **/
int find_max_weight_set(instance *inst, solution *sol, int ctr) {
    int set;
    for (; ctr < inst->n; ctr++) { // Start checking from the ctr'th set
        set = inst->sorted_by_weight[ctr];
        if (sol->x[set]) break; // Stop when the set is used in the solution
    }
    return ctr;
}


/*** Remove redundant sets from the solution ***/
void redundancy_elimination(instance *inst, solution *sol) {
    int tried=0, i, j, max_weight_set, can_remove, covered_by_set;
    int counter = sol->used_sets;
    while (counter) {
        counter--;
        tried = find_max_weight_set(inst, sol, tried);
        max_weight_set = inst->sorted_by_weight[tried];
        tried++;
        can_remove = 1;
        for (i = 0; i < inst->m; i++) {
            covered_by_set = 0;
            for (j = 0; j < sol->ncol_cover[i]; j++) { // Check if max_weight_set covers element i
                if(sol->col_cover[i][j] == max_weight_set) {
                    covered_by_set = 1;
                    break;
                }
            }
            if (covered_by_set && (sol->ncol_cover[i] == 1)) { // Don't remove the set if it's the only one that covers element i
                can_remove = 0;
                break;
            }
        }
        if (can_remove) {
            remove_set(inst, sol, max_weight_set);
        }
    }
    return;
}

/** Return maximum cost of sets in a solution **/
int max_cost(instance *inst, solution *sol) {
    int i;
    int max = 0;
    for (i = 0; i<inst->n; i++) {
        if (sol->x[i] && inst->cost[i] > max) {
            max = inst->cost[i];
        }
    }
    return max;
}

/** Add every set for which an element is only covered by that set **/
void column_inclusion(instance *inst,  solution *sol) {
    int i;
    for (i = 0; i<inst->m; i++) {
        if (inst->ncol[i] == 1) add_set(inst, sol, inst->col[i][0]);
    }
    return;
}


/** Free all memory of a solution **/
void free_solution(instance *inst, solution *sol) {
    int i;
    free((void *) sol->x);
    free((void *) sol->y);
    for (i = 0; i < inst->m; i++) {
        free((void *) sol->col_cover[i]);
    }
    free((void **) sol->col_cover);
    free((void *) sol->ncol_cover);
    free((void *) sol);
    return;
}