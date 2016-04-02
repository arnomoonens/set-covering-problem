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
#include "ils.h"
#include "aco.h"


/** Algorithm parameters **/
int seed=1234567;
char *scp_file="";

/** Variables to activate algorithms **/
int ch=0, bi=0, fi=0, re=0, ils=0, aco=0;


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
    printf("  --ils: iterated local search");
    printf("  --aco: ant colony optimization");
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
        } else if (strcmp(argv[i], "--ils") == 0) {
            ils=1;
        } else if (strcmp(argv[i], "--aco") == 0) {
            aco=1;
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
    
    if (ch == 0 && !ils && !aco) {
        printf("Error: exactly one of --ch1, --ch2, --ch3, --ch4, --ils and --aco needs to be provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    if (bi & fi) { //bitwise and
        printf("Error: maximallo one of --bi and --fi may be provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    if ((ils || aco) && (bi || fi)) {
        printf("Error: --bi and --fi can't be provided if --ils or --aco is provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    
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
    struct Solution *sol;
    read_parameters(argc, argv);
    srand(seed); /*set seed */
    instance = read_scp(scp_file);
    //print_instance(0);
    if (ch) {
        sol = initialize(instance);
        execute(instance, sol, ch, -1); // index of set to exclude is set to -1: don't exclude a set from possible being used
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
    } else if (ils) {
        sol = initialize(instance);
        double maxtime, T, TL, CF, ro1, ro2;
        ils_execute(instance, &sol, maxtime, T, TL, CF, ro1, ro2);
    } else {
        instance->sorted_by_weight = (int *) mymalloc(instance->n*sizeof(int));
        for (i = 0; i < instance->n; i++) instance->sorted_by_weight[i] = i;
        qsort(instance->sorted_by_weight, instance->n, sizeof(int), compare_cost);
        double maxtime = 120;
        sol = aco_execute(instance, maxtime, 20, 5.0, 0.99, 0.005);
    }
    printf("%i", sol->fx);
    finalize(sol);
    return EXIT_SUCCESS;
}
