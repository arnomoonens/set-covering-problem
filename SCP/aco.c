//
//  aco.c
//  SCP
//
//  Created by Arno Moonens on 31/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "aco.h"


extern double heuristic_information(instance *inst, ant *current_ant, int set);

/** Construction (SROM) phase of aco **/
void aco_construct(instance *inst, ant *current_ant, double *pheromones_trails, double beta) {
    int i, found_element, chosen_element, chosen_set_index, set;
    double denominator;
    double *pdf;
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
            if (current_ant->x[set] || inst->dominated_set[set]) continue;
            denominator += pheromones_trails[set]*pow(heuristic_information(inst, current_ant, set), beta);
        }
        for(i = 0; i < inst->n; i++) { // Calculate pdf itself
            if (set_covers_element(inst, i, chosen_element) && !current_ant->x[i] && !inst->dominated_set[i]) {
                pdf[i] = pheromones_trails[i]*pow(heuristic_information(inst, current_ant,i), beta) / denominator;
            } else {
                pdf[i] = 0;
            }
        }
        chosen_set_index = random_with_pdf(pdf, inst->n);
        add_set(inst, current_ant, inst->col[chosen_element][chosen_set_index]);
        free((void *) pdf);
    }
    return;
}

/** Heuristic information
 For a set, the heuristic information is the number of elements it covers extra divided by the cost of the set
 **/
double heuristic_information(instance *inst, ant *current_ant, int set) {
    return (double) current_ant->extra_covered[set] / (double) inst->cost[set];
}

/** Updating of pheromone trails of sets **/
void update_pheromone_trails(instance *inst, ant *global_best, double *pheromone_trails, double ro, double tau_min, double tau_max) {
    int i;
    double delta_tau = (double) 1 / (double) global_best->fx;
    for (i = 0; i < inst->n; i++) { // For each set
        if (inst->dominated_set[i]) continue;
        pheromone_trails[i] = ro * pheromone_trails[i] + ((double) global_best->x[i] * delta_tau); // If the set is not in the solution, delta_tau_i = 0
        if (pheromone_trails[i] < tau_min) {
            pheromone_trails[i] = tau_min;
        } else if (pheromone_trails[i] > tau_max) {
            pheromone_trails[i] = tau_max;
        }
    }
    return;
}


/** Local search phase of aco**/
void aco_local_search(instance *inst, ant *current_ant) {
    int j, set, element, lowest1 = 0, lowest2 = 0, nonly_covered_by_i;
    int *only_covered_by_i;
    int tried = 0;
    int counter = current_ant->used_sets;
    while (counter) {
        counter--;
        tried = find_max_weight_set(inst, current_ant, tried); // Consider columns from highest to lowest cost
        set = inst->sorted_by_weight[tried];
        tried++;
        nonly_covered_by_i = 0; // |W_j| in the paper
        only_covered_by_i = (int *) mymalloc(inst->nrow[set]*sizeof(int)); // W_j in the paper
        for (j = 0; j < inst->nrow[set]; j++) {
            element = inst->row[set][j];
            if(current_ant->ncol_cover[element] == 1 && current_ant->col_cover[element][0] == set) {
                only_covered_by_i[nonly_covered_by_i] = element;
                nonly_covered_by_i++;
            }
        }
        if (nonly_covered_by_i == 1 || nonly_covered_by_i == 2) lowest1 = lowest_covering_set(inst, only_covered_by_i[0]);
        if (nonly_covered_by_i == 2) lowest2 = lowest_covering_set(inst, only_covered_by_i[1]);

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

/** Execute aco algorithm **/
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
//            column_inclusion(inst, ants[i]); // Add sets that always need to be included
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
    // free all arrays
    free((void *) pheromones_trails);
    free((void *) ants);
    return global_best;
}