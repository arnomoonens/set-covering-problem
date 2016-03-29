/**      Heuristic Optimization     **/
/**              2016               **/
/**           Exercise 1            **/
/**       Set Covering Problem      **/
/**                                 **/
/*************************************/
/** For this code:                  **/
/**   rows = elements               **/
/**   cols = subsets                **/
/*************************************/

/** Code implemented for Heuritics optimization class by:  **/
/** Arno Moonens                                           **/

/** Note: Remember to keep your code in order and properly commented. **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "instance.h"
#include "solution.h"
#include "utils.h"
#include "complete.h"
#include "improvement.h"


/** Algorithm parameters **/
int seed=1234567;
char *scp_file="";

/** Variables to activate algorithms **/
int ch=0, bi=0, fi=0, re=0;


struct Instance *instance;


void usage(){
    printf("\nUSAGE: lsscp [param_name, param_value] [options]...\n");
    printf("Parameters:\n");
    printf("  --seed : seed to initialize random number generator\n");
    printf("  --instance: SCP instance to execute.\n");
    printf("Options:\n");
    printf("  --ch1: random solution construction\n");
    printf("  --ch2: static cost-based greedy values.\n");
    printf("  --ch3: static cover cost-based greedy values.\n");
    printf("  --ch4: Adaptive cover cost-based greedy values.\n");
    printf("  --re: applies redundancy elimination after construction.\n");
    printf("  --bi: best improvement.\n");
    printf("  --fi: first improvement.\n");
    printf("\n");
}



/*Read parameters from command line*/
/*NOTE: Complete parameter list*/
void read_parameters(int argc, char *argv[]) {
    int i;
    if(argc<=1) {
        usage();
        exit(EXIT_FAILURE);
    }
    for(i=1;i<argc;i++){
        if (strcmp(argv[i], "--seed") == 0) {
            seed=atoi(argv[i+1]);
            i+=1;
        } else if (strcmp(argv[i], "--instance") == 0) {
            scp_file=argv[i+1];
            i+=1;
        } else if (strcmp(argv[i], "--ch1") == 0) {
            ch=1;
        } else if (strcmp(argv[i], "--ch2") == 0) {
            ch=2;
        } else if (strcmp(argv[i], "--ch3") == 0) {
            ch=3;
        } else if (strcmp(argv[i], "--ch4") == 0) {
            ch=4;
        } else if (strcmp(argv[i], "--bi") == 0) {
            bi=1;
        } else if (strcmp(argv[i], "--fi") == 0) {
            fi=1;
        } else if (strcmp(argv[i], "--re") == 0) {
            re=1;
        } else {
            printf("\nERROR: parameter %s not recognized.\n",argv[i]);
            usage();
            exit( EXIT_FAILURE );
        }
    }
    
    if( (scp_file == NULL) || ((scp_file != NULL) && (scp_file[0] == '\0'))){
        printf("Error: --instance must be provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    
    if (ch == 0) {
        printf("Error: exactly one of --ch1, --ch2, --ch3 and --ch4 needs to be provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    
    if (bi & fi) { //bitwise and
        printf("Error: maximally one of --bi and --fi may be provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    
}


/** Choose a set according to the chosen algorithm(s) **/
int choose_set(struct Solution *sol, int exclude_set) {
    int i;
    if (ch1) {
        int found_element = 0;
        int chosen_element = 0;
        while (!found_element) {
            chosen_element = rand() % instance->m;
            if(!sol->y[chosen_element]) found_element = 1;
        }
        int chosen_set = exclude_set;
        while (chosen_set == exclude_set) {
            chosen_set = instance->col[chosen_element][rand() % instance->ncol[chosen_element]];
        }
        return chosen_set;
    } else if (ch2 || ch3 || ch4) {
        int best_set = 0;
        float best_cost = -1;
        float current_cost;
        int extra_covered;
        for (i = 0; i < instance->n; i++) {
            extra_covered = added_elements(instance, sol, i);
            if(extra_covered == 0 || i == exclude_set) continue; //Skip the set if it doesn't cover elements that weren't covered yet
            //Calculate cost according to chosen algorithm
            if(ch2) current_cost = instance->cost[i];
            else if(ch3) current_cost = (float) instance->cost[i] / (float) instance->nrow[i];
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
void execute(struct Instance *instance, struct Solution *sol, int exclude_set) {
    int chosen_set, element, i;
    while (uncovered_elements(instance, sol)) {
        chosen_set = choose_set(sol, exclude_set); //Choose set according to the construction heuristic
        for (i = 0; i < instance->nrow[chosen_set]; i++) { //Say that we cover each element that the chosen set covers
            element = instance->row[chosen_set][i];
            sol->y[element] = 1;
            sol->col_cover[element][sol->ncol_cover[element]] = chosen_set;
            sol->ncol_cover[element]++;
        }
        sol->used_sets++;
        sol->x[chosen_set] = 1;
        sol->fx += instance->cost[chosen_set];
    }
    return;
}


int find_max_weight_set(struct Solution *sol, int ctr) {
    int set;
    for (; ctr < instance->n; ctr++) { // Start checking from the ctr'th set
        set = instance->sorted_by_weight[ctr];
        if (sol->x[set]) break; // Stop when the set is used in the solution
    }
    return ctr;
}


/*** Remove redundant sets from the solution***/
void redundancy_elimination(struct Instance *instance, struct Solution *sol) {
    int i, j, max_weight_set, can_remove, covered_by_set;
    int tried = 0;
    int counter = sol->used_sets;
    while (counter) {
        counter--;
        tried = find_max_weight_set(sol, tried);
        max_weight_set = instance->sorted_by_weight[tried];
        tried++;
        can_remove = 1;
        for (i = 0; i < instance->m; i++) {
            covered_by_set = 0;
            for (j = 0; j < sol->ncol_cover[i]; j++) {
                if(sol->col_cover[i][j] == max_weight_set) {
                    covered_by_set = 1;
                    break;
                }
            }
            if (covered_by_set && (sol->ncol_cover[i] == 1)) {
                can_remove = 0;
                break;
            }
        }
        if (can_remove) {
            remove_set(instance, sol, max_weight_set);
        }
    }
    return;
}


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
    ch1 = ch2 = ch3 = 0;
    ch4 = 1;
    struct Solution ** best_solution = sol;
    while (improvement) {
        tried = 0;
        improvement = 0;
        int counter = (*best_solution)->used_sets;
        struct Solution *current_best = copy_solution(instance, *best_solution);
        while (counter) {
            counter--;
            tried = find_max_weight_set(*best_solution, tried); //get set with highest cost starting from tried'th set
            max_weight_set = instance->sorted_by_weight[tried];
            tried++;
            struct Solution *new_sol = copy_solution(instance, *best_solution);
            remove_set(instance, new_sol, max_weight_set); //Remove set from the new solution...
            execute(instance, new_sol, max_weight_set); //And build up the solution again without using the removed set
            if(new_sol->fx < current_best->fx) { //if move is new best improvement
                improvement = 1;
                free_solution(current_best);
                current_best = new_sol; //memorize move
            }
        }
        if (improvement) {
            free_solution(*best_solution);
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
    ch1 = ch2 = ch3 = 0;
    ch4 = 1;
    struct Solution ** best_solution = sol;
    while (improvement) {
        tried = 0;
        improvement = 0;
        int counter = (*best_solution)->used_sets;
        while (counter) {
            counter--;
            tried = find_max_weight_set(*best_solution, tried); //get set with highest cost starting from tried'th set
            max_weight_set = instance->sorted_by_weight[tried];
            tried++;
            struct Solution *new_sol = copy_solution(instance, *best_solution);
            remove_set(instance, new_sol, max_weight_set);
            execute(instance, new_sol, max_weight_set);
            if(new_sol->fx < (*best_solution)->fx) { //move improves
                improvement = 1;
                free_solution(*best_solution);
                *best_solution = new_sol; //apply move
                redundancy_elimination(instance, *best_solution);
                break;
            }
        }
    }
    return;
}

/*** Use this function to finalize execution */
void finalize(struct Solution *sol){
    free((void **) instance->row );
    free((void **) instance->col );
    free((void *) instance->nrow );
    free((void *) instance->ncol );
    free((void *) instance->cost );
    free((void *) instance);
    free_solution(sol);
}

/** Compare the cost of 2 solutions **/
int compare_cost(const void * a, const void * b)
{
    return ( instance->cost[*(int*)b] - instance->cost[*(int*)a] ); //cost of b - cost of a: sort from high to low cost
}


int main(int argc, char *argv[]) {
    int i;
    read_parameters(argc, argv);
    srand(seed); /*set seed */
    instance = read_scp(scp_file);
    //print_instance(0);
    struct Solution *sol = initialize(instance);
    execute(instance, sol, -1); // index of set to exclude is set to -1: don't exclude a set from possible being used
    if (re || bi || fi) {
        // Sort sets using cost from high to low
        instance->sorted_by_weight = (int *) mymalloc(instance->n*sizeof(int));
        for (i = 0; i < instance->n; i++) instance->sorted_by_weight[i] = i;
        qsort(instance->sorted_by_weight, instance->n, sizeof(int), compare_cost);
        if (re) redundancy_elimination(instance, sol);
        if (bi) best_improvement(instance, &sol);
        else if (fi) first_improvement(instance, &sol);
        free((void *) instance->sorted_by_weight);
    }
    printf("%i", sol->fx);
    finalize(sol);
    return EXIT_SUCCESS;
}