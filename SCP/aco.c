//
//  aco.c
//  SCP
//
//  Created by Arno Moonens on 31/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "aco.h"



/** Workings of construction:
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