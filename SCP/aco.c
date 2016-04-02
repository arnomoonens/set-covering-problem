//
//  aco.c
//  SCP
//
//  Created by Arno Moonens on 31/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "aco.h"



/** Workings of construction (SROM):
 while(uncovered_elements(sol)):
    t++;
    Randomly select an uncovered row i
    Select a column j that covers i according to the probab. distribution p(j)=(tau*n)/sum_columnscoveringi(tau*n)

**/
void aco_construct(struct Instance *instance, struct Solution *sol, double *pheromones, double *heuristic_factor) {
    int i, found_element, chosen_element, chosen_set;
    double *pdf;
    double denominator;
    pdf = (double *) mymalloc(instance->n * sizeof(double));
    while (uncovered_elements(instance, sol)) {
        found_element = 0;
        chosen_element = 0;
        while (!found_element) { // Randomly choose an element that isn't covered yet
            chosen_element = rand() % instance->m;
            if(!sol->y[chosen_element]) found_element = 1;
        }
        denominator = 0;
        for(i = 0; i < instance->ncol[chosen_element]; i++) { // Calculate denominator (used in pdf)
            denominator += pheromones[instance->col[chosen_element][i]]*heuristic_factor[instance->col[chosen_element][i]]; //multiply heuristic factor by beta (and pher. by alfa?)
        }
        for(i = 0; i < instance->n; i++) { // Calculate pdf itself
            if (set_covers_element(instance, i, chosen_element)) {
                pdf[i] = pheromones[i]*heuristic_factor[i] / denominator;
            } else {
                pdf[i] = 0;
            }
        }
        chosen_set = random_with_pdf(pdf, instance->n);
        add_set(instance, sol, chosen_set);
    }
    return;
}


/** Updating of pheromone trails
 (S_gb = globally best solution)
 Tau_j = ro*Tau_j + deltaTau_j, for each j element of J
 deltaTau_j = 1/sum(c_q for q in S_gb), if j in S_gb
            = 0, otherwise
 if Tau_j < Tau_min, then Tau_j = Tau_min
 if Tau_j > Tau_max, then Tau_j = Tau_max
 
 0 <= ro < 1: pheromone persistence
 Tau_max is an estimate of the asymptotically maximum value of pheromone trails, i.e Tau_max = 1/(1-ro)(sum(c_j for each j in S_gb)
 Each time a new best solution is found, Tau_max is updated
 Tau_min is set to epsilon*Tau_max, where epsilon (with 0 < epsilon < 1) is a ratio coefficient.
 Pheromone trails are initialized to an arbitrary high value for the purpose of achieving a higher exploration of solution space at the beginning of the algorithm.
**/


/** Heuristic information (subgradient)
 
 
 **/


/** Local search
 For each set j in the list of sets sorted by cost (highest->lowest):
    Compute W_j: elements only covered by set j
    If |W_j| = 0:
        set is redundant, remove it
    else if |W_j| = 1 and j /= low_q (low_q = set with lowest cost covering element q, only element of W_j):
        replace j with low_q
    else if |W_j| = 2 and j /= low_q1 = low_q2 (set with minimal cost covering element q1 is the same as the one covering q2, but different from j):
        replace j with low_q1
    else if |W_j| = 2 and low_q1 /= low_q2 and c_lowq1 + c_lowq2 <= c_j (sum of costs of low_q1 and low_q2 is lower than cost of j):
        replace j with low_q1 and low_q2
    else:
        continue
**/

void aco_local_search(struct Instance *instance, struct Solution *sol) {
    int i, set, element, j, lowest1 = 0, lowest2 = 0;
    for (i = 0; i < instance->n; i++) {
        i = find_max_weight_set(instance, sol, i);
        set = instance->sorted_by_weight[i];
        int nonly_covered_by_i = 0;
        int *only_covered_by_i = (int *) mymalloc(instance->nrow[set]*sizeof(int));
        for (element = 0; element < instance->m; element++) {
            for (j = 0; sol->ncol_cover[element]; j++) {
                if (sol->col_cover[element][j] == set) {
                    if (sol->ncol_cover[element] == 1) {
                        only_covered_by_i[nonly_covered_by_i] = element;
                        nonly_covered_by_i++;
                    }
                    break;
                }
            }
        }
        if (nonly_covered_by_i == 1) lowest1 = lowest_covering_set(instance, sol, only_covered_by_i[0]); // Can be assigned in the first else if?
        if (nonly_covered_by_i == 2) lowest2 = lowest_covering_set(instance, sol, only_covered_by_i[1]); // Can be assigned in the second else if?
        if (nonly_covered_by_i == 0) {
            remove_set(instance, sol, set);
        } else if (nonly_covered_by_i == 1 && (set != lowest1)) {
            remove_set(instance, sol, set);
            add_set(instance, sol, lowest1);
        } else if (nonly_covered_by_i == 2 && lowest1 == lowest2 && set != lowest1) {
            remove_set(instance, sol, set);
            add_set(instance, sol, lowest1);
        } else if (nonly_covered_by_i == 2 && instance->cost[lowest1] + instance->cost[lowest2] <= instance->cost[set]) {
            remove_set(instance, sol, set);
            add_set(instance, sol, lowest1);
            add_set(instance, sol, lowest2);
        }
    }
}

/** Total framework
 1. Get a Lagrangian multiplier vector with subgradient method
 2. Initialize pheromone trails and related parameters
 3. while termination condition is not met:
 4.     for each ant i:
 5.         Construct a solution S based on SROM
 6.         Apply local search to solution S optionally
 7.     Update pheromone trails
 8.     If the best solution is not improved for p consecutive iterations:
 9.         Perform subgradient method and get a new Lagrangian multiplier vector
**/