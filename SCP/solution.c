//
//  solution.c
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "solution.h"

/*** Use this function to initialize other variables of the algorithms **/
struct Solution *initialize(struct Instance *instance) {
    struct Solution *sol = mymalloc(sizeof(struct Solution));
    int i, j;
    sol->x = (int *) mymalloc(instance->n*sizeof(int));
    for (i = 0; i < instance->n; i++) {sol->x[i] = 0;}
    sol->y = (int *) mymalloc(instance->m*sizeof(int));
    for (i = 0; i < instance->m; i++) {sol->y[i] = 0;}
    
    sol->col_cover = (int **) mymalloc(instance->m*sizeof(int *));
    for (i=0; i<instance->m; i++) {
        sol->col_cover[i] = (int *) mymalloc(instance->ncol[i]*sizeof(int));
        for (j = 0; j < instance->ncol[i]; j++) {
            sol->col_cover[i][j] = -1;
        }
    }
    
    sol->ncol_cover = (int *) mymalloc(instance->m*sizeof(int));
    for (i = 0; i<instance->m; i++) {sol->ncol_cover[i] = 0;}
    sol->fx = 0;
    sol->used_sets = 0;
    return sol;
}


struct Solution *copy_solution(struct Instance *instance, struct Solution *source) {
    int i;
    struct Solution *new_sol = initialize(instance);
    new_sol->used_sets = source->used_sets;
    new_sol->fx = source->fx;
    
    memcpy(new_sol->x, source->x, instance->n * sizeof(int));
    memcpy(new_sol->y, source->y, instance->m * sizeof(int));
    
    for (i=0; i<instance->m; i++) {
        memcpy(new_sol->col_cover[i], source->col_cover[i], source->ncol_cover[i] * sizeof(int));
    }
    memcpy(new_sol->ncol_cover, source->ncol_cover, instance->m * sizeof(int));
    
    return new_sol;
}


void remove_set(struct Instance *instance, struct Solution *sol, int set) {
    int i, j, k;
    sol->used_sets--;
    sol->x[set] = 0;
    sol->fx -= instance->cost[set];
    for (i = 0; i < instance->m; i++) { //Check for each element...
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
int uncovered_elements(struct Instance *instance, struct Solution *sol) {
    int i;
    for (i = 0; i < instance->m; i++)
        if(!sol->y[i]) return 1;
    return 0;
}

/** Return how many yet uncovered elements the set would cover **/
int added_elements(struct Instance *instance, struct Solution *sol, int set) {
    int count = 0;
    int i;
    for (i = 0; i < instance->nrow[set]; i++)
        if (!sol->y[instance->row[set][i]]) count++;
    return count;
}

/** Return maximum cost of sets in a solution **/
int max_cost(struct Instance *instance, struct Solution *sol) {
    int i;
    int max = 0;
    for (i = 0; i<instance->n; i++) {
        if (sol->x[i] && instance->cost[i] > max) {
            max = instance->cost[i];
        }
    }
    return max;
}

/** Free all memory of a solution **/
void free_solution(struct Solution *sol) {
    free((void *) sol->x);
    free((void *) sol->y);
    free((void **) sol->col_cover);
    free((void *) sol->ncol_cover);
    free((void *) sol);
    return;
}