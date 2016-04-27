//
//  aco.c
//  SCP
//
//  Created by Arno Moonens on 31/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "aco.h"


extern double heuristic_information(instance *inst, ant *current_ant, int set);

/** Workings of construction (SROM):
 while(uncovered_elements(sol)):
    Randomly select an uncovered row i
    Select a column j that covers i according to the probab. distribution p(j)=(tau_j*n_j)/sum_columnscoveringi(tau_k*n_k)

**/
void aco_construct(instance *inst, ant *current_ant, double *pheromones_trails, double beta) {
    int i, found_element, chosen_element, chosen_set, set;
    double denominator;
    double *pdf = (double *) mymalloc(inst->n * sizeof(double));
    while (uncovered_elements(inst, current_ant)) {
        found_element = 0;
        chosen_element = 0;
        while (!found_element) { // Randomly choose an element that isn't covered yet
            chosen_element = rand() % inst->m;
            if(!current_ant->y[chosen_element]) found_element = 1;
        }
        denominator = 0;
        for(i = 0; i < inst->ncol[chosen_element]; i++) { // Calculate denominator (used in pdf)
            set = inst->col[chosen_element][i];
            if (current_ant->x[set]) continue;
            denominator += pheromones_trails[set]*pow(heuristic_information(inst, current_ant, set), beta);
        }
        for(i = 0; i < inst->n; i++) { // Calculate pdf itself
            if (set_covers_element(inst, i, chosen_element) && !current_ant->x[i]) {
                pdf[i] = (pheromones_trails[i]*pow(heuristic_information(inst, current_ant,i), beta)) / denominator;
            } else {
                pdf[i] = 0;
            }
        }
        chosen_set = random_with_pdf(pdf, inst->n);
        add_set(inst, current_ant, chosen_set);
    }
    free((void *) pdf);
    return;
}

/** Heuristic information
 For a set, the heuristic information is the number of elements it covers extra divided by the cost of the set
 **/
double heuristic_information(instance *inst, ant *current_ant, int set) {
    return (double) current_ant->extra_covered[set] / (double) inst->cost[set];
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
void update_pheromone_trails(instance *inst, ant *global_best, double *pheromone_trails, double ro, double tau_min, double tau_max) {
    int i;
    double delta_tau = (double) 1 / (double) global_best->fx;
    for (i = 0; i < inst->n; i++) { // For each set
        pheromone_trails[i] = ro * pheromone_trails[i] + ((double) global_best->x[i] * delta_tau); // If the set is not in the solution, delta_tau_i = 0
        if (pheromone_trails[i] < tau_min) {
            pheromone_trails[i] = tau_min;
        } else if (pheromone_trails[i] > tau_max) {
            pheromone_trails[i] = tau_max;
        }
    }
    return;
}


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
void aco_local_search(instance *inst, ant *current_ant) {
    int i, j, set, element, lowest1 = 0, lowest2 = 0, nonly_covered_by_i;
    int *only_covered_by_i;
    for (i = 0; i < inst->n; i++) {
        i = find_max_weight_set(inst, current_ant, i); // Consider columns from highest to lowest cost
        set = inst->sorted_by_weight[i];
        nonly_covered_by_i = 0; // |W_j| in the paper
        only_covered_by_i = (int *) mymalloc(inst->nrow[set]*sizeof(int)); // W_j in the paper
        for (j = 0; j < inst->nrow[set]; j++) {
            element = inst->row[set][j];
            if(current_ant->ncol_cover[element] == 1 && current_ant->col_cover[element][0] == set) {
                only_covered_by_i[nonly_covered_by_i] = element;
                nonly_covered_by_i++;
            }
        }
        if (nonly_covered_by_i == 1 || nonly_covered_by_i == 2) lowest1 = lowest_covering_set(inst, only_covered_by_i[0]); // Can be assigned in the first else if?
        if (nonly_covered_by_i == 2) lowest2 = lowest_covering_set(inst, only_covered_by_i[1]); // Can be assigned in the second else if?

        if (nonly_covered_by_i == 0) { // Set is redundant
            remove_set(inst, current_ant, set);
        } else if (nonly_covered_by_i == 1 && (set != lowest1)) {
            remove_set(inst, current_ant, set);
            add_set(inst, current_ant, lowest1);
        } else if (nonly_covered_by_i == 2 && set != lowest1 && lowest1 == lowest2) {
            remove_set(inst, current_ant, set);
            add_set(inst, current_ant, lowest1);
        } else if (nonly_covered_by_i == 2 && lowest1 != lowest2 && ((inst->cost[lowest1] + inst->cost[lowest2]) <= inst->cost[set])) {
            remove_set(inst, current_ant, set);
            add_set(inst, current_ant, lowest1);
            add_set(inst, current_ant, lowest2);
        }
        free((void *) only_covered_by_i);
    }
    return;
}

/** Total framework
 0. Apply column domination stuff
 1. Get a Lagrangian multiplier vector with subgradient method OR use simpler method of calculating heuristic information
 2. Initialize pheromone trails and related parameters
 3. while termination condition is not met:
 4.     for each ant i:
 5.         Construct a solution S based on SROM
 6.         Apply local search to solution S optionally
 7.     Update pheromone trails
(8.     If the best solution is not improved for p consecutive iterations:)
(9.         Perform subgradient method and get a new Lagrangian multiplier vector)
**/
solution * aco_execute(instance *inst, int (*termination_criterion)(solution *), void (*notify_improvement)(solution *), int nants, double beta, double ro, double epsilon) {
    int i, all_set_costs = 0, improvement;
    double tau_min, tau_max;
    ant *global_best = NULL;
    ant **ants = mymalloc(nants * sizeof(ant *));
    for (i = 0; i < inst->n; i++) all_set_costs += inst->cost[i];
    tau_max = (double) 1 / (((double) 1 - ro) * (double) all_set_costs); // Is total cost of all sets instead of cost of global best at the start
    tau_min = epsilon * tau_max;
    double *pheromones_trails = mymalloc(inst->n * sizeof(double));
    for (i = 0; i < inst->n; i++) pheromones_trails[i] = tau_max; // Set initial pheromone trails to tau_max
    while(!termination_criterion(global_best)) {
        improvement = 0;
        for (i = 0; i < nants; i++) { // For each ant...
            ants[i] = initialize(inst, 1);
            //column_inclusion(inst, ants[i]); // Add sets that always need to be included (see explanation above function)
            aco_construct(inst, ants[i], pheromones_trails, beta); // Construct a solution...
            aco_local_search(inst, ants[i]); /// And apply local search
            if (!global_best) {
                global_best = ants[i];
                improvement = 1;
                notify_improvement(ants[i]);
            } else if (ants[i]->fx <= global_best->fx) {
                if (ants[i]->fx < global_best->fx) notify_improvement(ants[i]);
                free_ant(inst, global_best);
                global_best = ants[i];
                improvement = 1;
            } else {
                free_ant(inst, ants[i]);
            }
        }
        if (improvement) {
            tau_max = (double) 1 / (((double) 1 - ro) * (double) global_best->fx);
            tau_min = epsilon * tau_max;
        }
        update_pheromone_trails(inst, global_best, pheromones_trails, ro, tau_min, tau_max);
    }
    //free all arrays
    free((void *) pheromones_trails);
    free((void *) ants);
    return global_best;
}