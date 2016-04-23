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
void best_improvement(struct Instance *inst, solution **sol, void (*notify_improvement)(solution *)) {
    int max_weight_set;
    int improvement = 1;
    int tried = 0;
    solution ** best_solution = sol;
    while (improvement) {
        tried = 0;
        improvement = 0;
        int counter = (*best_solution)->used_sets;
        solution *current_best = copy_solution(inst, *best_solution);
        while (counter) {
            counter--;
            tried = find_max_weight_set(inst, *best_solution, tried); //get set with highest cost starting from tried'th set
            max_weight_set = inst->sorted_by_weight[tried];
            tried++;
            solution *new_sol = copy_solution(inst, *best_solution);
            remove_set(inst, new_sol, max_weight_set); //Remove set from the new solution...
            execute(inst, new_sol, 4, max_weight_set); //And build up the solution again with CH4, without using the removed set
            if(new_sol->fx < current_best->fx) { //if move is new best improvement
                improvement = 1;
                free_solution(inst, current_best);
                current_best = new_sol; //memorize move
            }
        }
        if (improvement) {
            //notify_improvement(current_best);
            free_solution(inst, *best_solution);
            *best_solution = current_best; //apply best move
            redundancy_elimination(inst, *best_solution);
        }
    }
    return;
}

void first_improvement(struct Instance *inst, solution **sol, void (*notify_improvement)(solution *)) {
    int max_weight_set;
    int improvement = 1;
    int tried = 0;
    solution ** best_sol = sol;
    while (improvement) {
        tried = 0;
        improvement = 0;
        int counter = (*best_sol)->used_sets;
        while (counter) {
            counter--;
            tried = find_max_weight_set(inst, *best_sol, tried); //get set with highest cost starting from tried'th set
            max_weight_set = inst->sorted_by_weight[tried];
            tried++;
            solution *new_sol = copy_solution(inst, *best_sol);
            remove_set(inst, new_sol, max_weight_set);
            execute(inst, new_sol, 4, max_weight_set); // rebuild solution
            if(new_sol->fx < (*best_sol)->fx) { //move improves
                //notify_improvement(new_sol);
                improvement = 1;
                free_solution(inst, *best_sol);
                *best_sol = new_sol; //apply move
                redundancy_elimination(inst, *best_sol);
                break;
            }
        }
    }
    return;
}