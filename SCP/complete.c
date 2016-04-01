//
//  complete.c
//  SCP
//
//  Created by Arno Moonens on 29/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "complete.h"

/** Choose a set according to the chosen algorithm(s) **/
int choose_set(struct Instance *instance, struct Solution *sol, int ch, int exclude_set) {
    int i;
    if (ch == 1) {
        int found_element = 0; //0 or 1: says whether a good element is found or not
        int chosen_element = 0; //actual id of the element
        while (!found_element) {
            chosen_element = rand() % instance->m;
            if(!sol->y[chosen_element]) found_element = 1;
        }
        int chosen_set = exclude_set;
        while (chosen_set == exclude_set) {
            chosen_set = instance->col[chosen_element][rand() % instance->ncol[chosen_element]];
        }
        return chosen_set;
    } else if (ch >= 2 && ch <= 4) {
        int best_set = 0;
        float best_cost = -1;
        float current_cost;
        int extra_covered;
        for (i = 0; i < instance->n; i++) {
            extra_covered = added_elements(instance, sol, i);
            if(extra_covered == 0 || i == exclude_set) continue; //Skip the set if it doesn't cover elements that weren't covered yet
            //Calculate cost according to chosen algorithm
            if(ch == 2) current_cost = instance->cost[i];
            else if(ch == 3) current_cost = (float) instance->cost[i] / (float) instance->nrow[i];
            else current_cost = (float) instance->cost[i] / (float) extra_covered;
            
            if (current_cost < best_cost && !sol->x[i] && best_cost >= 0) { //Cost of set we're handling is better than the best one we currently have
                best_set = i;
                best_cost = current_cost;
            } else if (current_cost == best_cost && instance->nrow[i] > instance->nrow[best_set] && !sol->x[i] && best_cost >= 0) { //Cost is equal but new one covers more elements
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
        return rand() % instance->n;
    }
}

/*** Build solution ***/
void execute(struct Instance *instance, struct Solution *sol, int ch, int exclude_set) {
    int chosen_set;
    while (uncovered_elements(instance, sol)) {
        chosen_set = choose_set(instance, sol, ch, exclude_set); //Choose set according to the construction heuristic
        add_set(instance, sol, chosen_set);
    }
    return;
}