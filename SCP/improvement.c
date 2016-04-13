//
//  improvement.c
//  SCP
//
//  Created by Arno Moonens on 29/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "improvement.h"


/**Working of best_improvement:
 - Find set with i'th highest cost that's used and remove it
 - Use adapted execute function that doesn't use removed set
 - Calculate and save cost
 - keep solution with lowest cost
 **/
void best_improvement(struct Instance *instance, struct Solution **sol) {
    int max_weight_set;
    int improvement = 1;
    int tried = 0;
    struct Solution ** best_solution = sol;
    while (improvement) {
        tried = 0;
        improvement = 0;
        int counter = (*best_solution)->used_sets;
        struct Solution *current_best = copy_solution(instance, *best_solution);
        while (counter) {
            counter--;
            tried = find_max_weight_set(instance, *best_solution, tried); //get set with highest cost starting from tried'th set
            max_weight_set = instance->sorted_by_weight[tried];
            tried++;
            struct Solution *new_sol = copy_solution(instance, *best_solution);
            remove_set(instance, new_sol, max_weight_set); //Remove set from the new solution...
            execute(instance, new_sol, 4, max_weight_set); //And build up the solution again with CH4, without using the removed set
            if(new_sol->fx < current_best->fx) { //if move is new best improvement
                improvement = 1;
                free_solution(instance, current_best);
                current_best = new_sol; //memorize move
            }
        }
        if (improvement) {
            free_solution(instance, *best_solution);
            *best_solution = current_best; //apply best move
            redundancy_elimination(instance, *best_solution);
        }
    }
    return;
}

void first_improvement(struct Instance *instance, struct Solution **sol) {
    int max_weight_set;
    int improvement = 1;
    int tried = 0;
    struct Solution ** best_solution = sol;
    while (improvement) {
        tried = 0;
        improvement = 0;
        int counter = (*best_solution)->used_sets;
        while (counter) {
            counter--;
            tried = find_max_weight_set(instance, *best_solution, tried); //get set with highest cost starting from tried'th set
            max_weight_set = instance->sorted_by_weight[tried];
            tried++;
            struct Solution *new_sol = copy_solution(instance, *best_solution);
            remove_set(instance, new_sol, max_weight_set);
            execute(instance, new_sol, 4, max_weight_set);
            if(new_sol->fx < (*best_solution)->fx) { //move improves
                improvement = 1;
                free_solution(instance, *best_solution);
                *best_solution = new_sol; //apply move
                redundancy_elimination(instance, *best_solution);
                break;
            }
        }
    }
    return;
}