// Iterated local search
//  ils.c
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "ils.h"

/** How it works:
 
 MT = maximum CPU time;
 T = initial temperature, a parameter which controls the probability of accepting an inferior solution;
 TL = temperature length, the number of iterations at a particular value of T;
 CF= coolingfactor; the percentage by which T is reduced after TL iterations;
 
 CONSTRUCT a feasible solution S with cost Z(S)
 set T, TL, and CF
 set the maximum run time, MT
 read the current clock time, ctime
 do while ctime<MT
 do i= 1, TL
 SEARCH for a neighbor S’ of S
 let Z(S’) = the cost of S’
 if Z(S’) <= Z(S)
 let S = S’
 let S* = S’
 else
 let S = S’ with probability e^(-6/T)
 end if
 continue
 let T= T * CF
 read ctime
 continue
 return the minimum cost solution S* and Z (S*)
 end
 
 
 SEARCH:
 0. Set d=0,D=ceiling(ro1*N(S)),and E=ceiling(ro2*Q(S))
 1. Randomly select a set k, with k element of the solution.
 2. Remove this set, increment d. If d=D go to 3, else go to 1
 3. if uncovered_elements(S), go to 4 else go to 6
 4. Define RE as the set of sets which have a cost <= E. Compute the following
 a_ij= 1,if w_i=0 and a_ij= 1 for all i element ofI,j element of RE;0,Otherwise
 a_j= sum of a_ij for all j element of RE;
 Beta_j = c_j/a_j for all j element of RE.
 Beta_min = min_j(Beta_j | j element of RE)
 K = set of columns for which Beta_j = Beta_min | j element of RE.
 5. Randomly select a column k element of K and add that set to the solution. Return to step 3.
 6. Apply redundancy elimination
 **/

void ils_search(struct Instance *instance, struct Solution *sol, double ro1, double ro2) {
    int i, set;
    double D = ceil(((double) sol->used_sets) * ro1); //number of sets to remove
    double E = ceil(((double) max_cost(instance, sol)) * ro2); //maximum cost of candidate sets for rebuilding solution
    for (i = 0; i < D; i++) { //Randomly remove D sets
        set = -1;
        while (!sol->x[set]) {
            set = rand() % instance->n;
        }
        remove_set(instance, sol, set);
    }
    while (uncovered_elements(instance, sol)) { //Rebuild solution
        int *candidate_sets = (int *) mymalloc(instance->n*sizeof(int));
        for (i = 0; i < instance->n; i++) {
            candidate_sets[i] = !sol->x[i] && instance->cost[i] <= E; //A set is still a candidate if it's unused and its cost <= E
        }
        float *alfa = (float *) mymalloc(instance->n*sizeof(float));
        float min_alfa = -1;
        for (i = 0; i < instance->n; i++) {
            if (candidate_sets[i]) {
                alfa[i] = (float) instance->cost[i] / (float) added_elements(instance, sol, i); //same value as in CH4
                if (min_alfa < 0) {
                    min_alfa = alfa[i];
                } else if (alfa[i] < min_alfa) {
                    min_alfa = alfa[i];
                }
            }
        }
        for (i = 0; i < instance->n; i++) { //A candidate set remains a candidate if its alfa is minimal
            candidate_sets[i] = candidate_sets[i] && (alfa[i] == min_alfa);
        }
        int chosen_set = -1;
        while (chosen_set < 0 || !candidate_sets[chosen_set]) { //Randomly choose out of the candidate sets
            chosen_set = rand() % instance->n;
            add_set(instance, sol, chosen_set);
        }
    }
    redundancy_elimination(instance, sol); //add after refactoring
}


void ils_execute(struct Instance *instance, struct Solution **sol, double maxtime, double T, double TL, double CF, double ro1, double ro2) {
    int i;
    time_t starttime = time(0);
    struct Solution **overall_best = sol;
    while(difftime(time(0), starttime) < maxtime) {
        struct Solution *current_best = copy_solution(instance, *sol);
        for (i = 0; i < TL; i++) {
            struct Solution *new_sol = copy_solution(instance, current_best);
            ils_search(instance, new_sol, ro1, ro2);
            int delta = new_sol->fx - current_best->fx;
            if (delta <= 0) {
                free_solution(*overall_best);
                free_solution(current_best);
                overall_best = &new_sol;
                current_best = new_sol;
            } else if (random_with_probability(exp(((double) delta)/T))) {
                free_solution(current_best);
                current_best = new_sol;
            }
        }
        T = T*CF;
    }
    return;
}