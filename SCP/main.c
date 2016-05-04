/**      Heuristic Optimization     **/
/**              2016               **/
/**          Arno Moonens           **/
/**       Set Covering Problem      **/
/**                                 **/
/*************************************/
/** For this code:                  **/
/**   rows = elements               **/
/**   cols = subsets                **/
/*************************************/

/** Code implemented for Heuritics optimization class by:  **/
/** Arno Moonens                                           **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>

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
int ch=0, bi=0, fi=0, re=0, ils=0, aco=0, iterations=0;
double mt=0, mc=0, co=0;
struct timeval *start_time = NULL;
char *trace_file="";

instance *inst;

/** Print arguments that can be used when calling the program **/
void usage(){
    printf("\nUSAGE: lsscp [param_name, param_value] [options]...\n");
    printf("Parameters:\n");
    printf("  --seed : seed to initialize random number generator\n");
    printf("  --instance: SCP instance to execute.\n");
    printf("Options:\n");
    printf("  --ch1: random solution construction\n");
    printf("  --ch2: static cost-based greedy values.\n");
    printf("  --ch3: static cover cost-based greedy values.\n");
    printf("  --ch4: adaptive cover cost-based greedy values.\n");
    printf("  --re: applies redundancy elimination after construction.\n");
    printf("  --bi: best improvement.\n");
    printf("  --fi: first improvement.\n");
    printf("  --ils: iterated local search.\n");
    printf("  --aco: ant colony optimization.\n");
    printf("  --mt: maximum time to run --ils or --aco.\n");
    printf("  --mc: maximum cost of the solution obtained by --ils or --aco.\n");
    printf("  --co: cut-off time when using --mc.\n");
    printf("  --trace: output time/cost traces on improvement to the given file.\n");
    printf("\n");
}

/* Read parameters from command line */
void read_parameters(int argc, char *argv[]) {
    int i;
    if(argc<=1) {
        usage();
        exit(EXIT_FAILURE);
    }
    for(i=1;i<argc;i++){
        if (strcmp(argv[i], "--seed") == 0) {
            seed=atoi(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--instance") == 0) {
            scp_file=argv[i+1];
            i++;
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
        } else if (strcmp(argv[i], "--mt") == 0) {
            sscanf(argv[i+1], "%lf", &mt);
            i++;
        } else if (strcmp(argv[i], "--mc") == 0) {
            sscanf(argv[i+1], "%lf", &mc);
            i++;
        } else if (strcmp(argv[i], "--co") == 0) {
            sscanf(argv[i+1], "%lf", &co);
            i++;
        } else if (strcmp(argv[i], "--trace") == 0) {
            trace_file=argv[i+1];
            i++;
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
    if (bi & fi) { // bitwise and
        printf("Error: maximally one of --bi and --fi may be provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }
    if ((ils || aco) && (bi || fi)) {
        printf("Error: --bi and --fi can't be provided if --ils or --aco is provided.\n");
        usage();
        exit( EXIT_FAILURE );
    }

    if ((ils || aco) && ((mt <= 0 && mc <= 0) || (mt && mc))) {
        printf("Error: When using --ils or --aco, an --mt OR --mc of greater than 0 must be provided.\n");
        usage();
        exit(EXIT_FAILURE);
    }

    if ((mc || co) && !(mc && co)) {
        printf("Error: --mc and --co must be used together.\n");
        usage();
        exit(EXIT_FAILURE);
    }

}


/*** Finalize execution by freeing variables */
void finalize(solution *sol){
    int i;
    free_solution(inst, sol);
    for (i = 0; i < inst->n; i++) {
        free((void *)inst->row[i]);
    }
    free((void **) inst->row );
    for (i = 0; i < inst->m; i++) {
        free((void *)inst->col[i]);
    }
    free((void **) inst->col );
    free((void *) inst->nrow );
    free((void *) inst->ncol );
    free((void *) inst->cost );
    free((void *) inst);
}

/** Compare the cost of 2 solutions **/
int compare_cost(const void * a, const void * b)
{
    return ( inst->cost[*(int*)b] - inst->cost[*(int*)a] ); //cost of b - cost of a: sort from high to low cost
}

/** Make an array with the ids of set with decreasing cost **/
void sort_sets_descending() {
    int i;
    inst->sorted_by_weight = (int *) mymalloc(inst->n*sizeof(int));
    for (i = 0; i < inst->n; i++) inst->sorted_by_weight[i] = i;
    qsort(inst->sorted_by_weight, inst->n, sizeof(int), compare_cost);
    return;
}

/** Used by ils and aco to determine when to stop **/
int termination_criterion(solution *sol) {
    struct timeval now;
    if (!start_time) {
        start_time = (struct timeval *) mymalloc(sizeof(struct timeval));
        gettimeofday(start_time, NULL);
    }
    gettimeofday(&now, NULL);
//    if (sol) printf("\rIteration %i:%f / %f %i", ++iterations, mdifftime(&now, start_time), mt, sol->fx);
//    fflush(stdout);
    double time_elapsed = mdifftime(&now, start_time);
    return (mt && time_elapsed > mt) || (mc && ((sol && sol->fx <= mc) || time_elapsed > co));
}

/** Callback when better solution is encountered: write time and quality to file **/
void notify_improvement(solution *sol) {
    struct timeval now;
    gettimeofday(&now, NULL);
    if (!start_time) {
        start_time = (struct timeval *) mymalloc(sizeof(struct timeval));
        gettimeofday(start_time, NULL);
    }
    double time_elapsed = mdifftime(&now, start_time);
    FILE *fp = fopen(trace_file, "a" );
    if(strlen(trace_file) != 0 && fprintf(fp, "%f, %i\n", time_elapsed, sol->fx) < 0)
        error_writing_file("ERROR: there was an error writing the trace file.");
    fclose(fp);
    return;
}


int main(int argc, char *argv[]) {
    solution *sol;
    read_parameters(argc, argv);
    srand(seed); /* set seed */
    inst = read_scp(scp_file);
    //print_instance(0);
    if (ch) {
        sol = initialize(inst, 1);
        execute(inst, sol, ch, -1); // index of set to exclude is set to -1: don't exclude a set from possible being used
        if (re || bi || fi) {
            sort_sets_descending();
            if (re) redundancy_elimination(inst, sol);
            if (bi) best_improvement(inst, &sol, notify_improvement);
            else if (fi) first_improvement(inst, &sol, notify_improvement);
            free((void *) inst->sorted_by_weight);
        }
    } else if (ils) {
        sol = initialize(inst, 1); // Initialize an empty solution
        sort_sets_descending();
        execute(inst, sol, 4, -1); // Construct an initial solution using CH4
        redundancy_elimination(inst, sol);
        // Use PS3 settings from paper
        double T = 1.3, TL = 100, CF = 0.9, ro1 = 0.4, ro2 = 1.1;
        ils_execute(inst, &sol, termination_criterion, notify_improvement, T, TL, CF, ro1, ro2);
    } else {
        sort_sets_descending();
        double beta = 5.0, ro=0.99, epsilon=0.005;
        int nants = 20;
        sol = aco_execute(inst, termination_criterion, notify_improvement, nants, beta, ro, epsilon);
    }
    printf("%i", sol->fx);
    finalize(sol);
    return EXIT_SUCCESS;
}