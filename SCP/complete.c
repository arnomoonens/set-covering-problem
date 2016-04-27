//
//  complete.c
//  SCP
//
//  Created by Arno Moonens on 29/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "complete.h"

/** Choose a set according to the chosen algorithm ch, exluding the set exclude_set **/
int choose_set(instance *inst, solution *sol, int ch, int exclude_set) {
    int i;
    if (ch == 1) {
        int found_element = 0; //0 or 1: says whether a good element is found or not
        int chosen_element = 0; //actual id of the element
        while (!found_element) { //Try random elements until we find one that isn't already covered
            chosen_element = rand() % inst->m;
            if(!sol->y[chosen_element]) found_element = 1;
        }
        int chosen_set = exclude_set;
        while (chosen_set == exclude_set) { //Choose a random set until we find one that doesn't need to be excluded from the solution
            chosen_set = inst->col[chosen_element][rand() % inst->ncol[chosen_element]];
        }
        return chosen_set;
    } else if (ch >= 2 && ch <= 4) { // Using CH2, CH3 and CH4
        int best_set = 0;
        float best_cost = -1;
        float current_cost;
        int extra_covered;
        for (i = 0; i < inst->n; i++) {
            extra_covered = sol->extra_covered[i];
            if(extra_covered == 0 || i == exclude_set) continue; //Skip the set if it doesn't cover elements that weren't covered already
            //Calculate cost according to chosen algorithm
            if(ch == 2) current_cost = inst->cost[i];
            else if(ch == 3) current_cost = (float) inst->cost[i] / (float) inst->nrow[i];
            else current_cost = (float) inst->cost[i] / (float) extra_covered;
            
            if (current_cost < best_cost && !sol->x[i] && best_cost >= 0) { //Cost of set we're handling is better than the best one we currently have
                best_set = i;
                best_cost = current_cost;
            } else if (current_cost == best_cost && inst->nrow[i] > inst->nrow[best_set] && !sol->x[i] && best_cost >= 0) { //Cost is equal but new one covers more elements
                best_set = i;
                best_cost = current_cost;
            } else if (!sol->x[i] && (best_cost < 0)) { //Initialize index and cost with the first set that isn't already included (and isn't redundant)
                best_set = i;
                best_cost = current_cost;
            }
        }
        return best_set;
    } else {
        printf("No algorithm passed, choosing randomly\n");
        return rand() % inst->n;
    }
}

/*** Build solution ***/
void execute(instance *inst, solution *sol, int ch, int exclude_set) {
    int chosen_set;
    while (uncovered_elements(inst, sol)) {
        chosen_set = choose_set(inst, sol, ch, exclude_set); //Choose set according to the construction heuristic
        add_set(inst, sol, chosen_set); // Add the chosen set to the current solution
    }
    return;
}