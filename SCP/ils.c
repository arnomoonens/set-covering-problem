// Iterated local search
//  ils.c
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "ils.h"

/** Local search phase of ils algorithm **/
void ils_search(instance *inst, solution *sol, double ro1, double ro2) {
    int i, set;
    double D = ceil(((double) sol->used_sets) * ro1); // number of sets to remove
    double E = ceil(((double) max_cost(inst, sol)) * ro2); // maximum cost of candidate sets for rebuilding solution
    int *candidate_sets = (int *) mymalloc(inst->n*sizeof(int));
    float *alfa = (float *) mymalloc(inst->n*sizeof(float));
    for (i = 0; i < D; i++) { //Randomly remove D sets
        set = -1;
        while (set < 0 || !sol->x[set]) {
            set = rand() % inst->n;
        }
        remove_set(inst, sol, set);
    }
    while (uncovered_elements(inst, sol)) { // Rebuild solution
        for (i = 0; i < inst->n; i++) {
            candidate_sets[i] = !sol->x[i] && inst->cost[i] <= E; // A set is still a candidate if it's unused and its cost <= E
        }
        float min_alfa = -1;
        for (i = 0; i < inst->n; i++) {
            if (candidate_sets[i]) {
                alfa[i] = (float) inst->cost[i] / (float) sol->extra_covered[i]; // same value as in CH4
                if (min_alfa < 0) {
                    min_alfa = alfa[i];
                } else if (alfa[i] < min_alfa) {
                    min_alfa = alfa[i];
                }
            }
        }
        for (i = 0; i < inst->n; i++) { // A candidate set remains a candidate if its alfa is minimal
            candidate_sets[i] = candidate_sets[i] && (alfa[i] == min_alfa);
        }
        int chosen_set = -1;
        while (chosen_set < 0 || !candidate_sets[chosen_set]) { // Randomly choose out of the candidate sets
            chosen_set = rand() % inst->n;
        }
        add_set(inst, sol, chosen_set);
    }
    free((void *) candidate_sets);
    free((void *) alfa);
    redundancy_elimination(inst, sol);
    return;
}

/** Executes ils algorithm **/
void ils_execute(instance *inst, solution **sol, int (*termination_criterion)(solution *), void (*notify_improvement)(solution *), double T, double TL, double CF, double ro1, double ro2) {
    int i, delta;
    solution **overall_best = (solution **) mymalloc(sizeof(solution *));
    solution *new_sol;
    solution *current_best = *sol;
    *overall_best = current_best;
    while(!termination_criterion(*overall_best)) {
        for (i = 0; i < TL; i++) { // Keep a certain temperature T for temperature length TL times
            new_sol = copy_solution(inst, current_best);
            ils_search(inst, new_sol, ro1, ro2);
            delta = new_sol->fx - current_best->fx;
            if (delta <= 0) { // New solution is better or equal: always keep it as the current and overall best
                if (current_best != *overall_best) {
                    free_solution(inst, current_best);
                }
                current_best = new_sol;
                if (new_sol->fx < (*overall_best)->fx) { // Deviation from paper: only update overall best if it improves, instead of when current_best improves
                    notify_improvement(new_sol);
                    free_solution(inst, *overall_best);
                    *overall_best = new_sol;
                }
            } else if (random_with_probability(exp((-(double) delta)/T))) { // New solution is worse, keep it as the current 'best' with a probability
                if (current_best != *overall_best) {
                    free_solution(inst, current_best);
                }
                current_best = new_sol;
            } else free_solution(inst, new_sol);
        }
        T = T*CF; // TL iterations done, update temperature
    }
    *sol = *overall_best;
    free((void **) overall_best);
    return;
}