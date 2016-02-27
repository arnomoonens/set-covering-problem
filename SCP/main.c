/**      Heuristic Optimization     **/
/**              2016               **/
/**       Template exercise 1       **/
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
#include <time.h>
#include <math.h>
#include <string.h>
#include <limits.h>


//Following 2 functions were originally in utils.c
void *mymalloc( size_t size ) {
    void *s;
    if ( (s=malloc(size)) == NULL ) {
        fprintf( stderr, "malloc : Not enough memory.\n" );
        exit( EXIT_FAILURE );
    }
    return s;
}

void error_reading_file(char *text){
    printf("%s\n", text);
    exit( EXIT_FAILURE );
}


/** Algorithm parameters **/
int seed=1234567;
char *scp_file="";
char *output_file="output.txt";

/** Variables to activate algorithms **/
int ch1=0, ch2=0, ch3=0, ch4=0, bi=0, fi=0, re=0;

/** Instance static variables **/
int m;            /* number of rows = elements */
int n;            /* number of columns = sets */
int **row;        /* row[i] rows that are covered by column i */
int **col;        /* col[i] columns that cover row i */
int *ncol;        /* ncol[i] number of columns that cover row i */
int *nrow;        /* nrow[i] number of rows that are covered by column i */
int *cost;        /* cost[i] cost of column i  */

/** Holds all solution information **/
struct Solution {
    int *x;           /* x[i] 0,1 if column i is selected */
    int *y;           /* y[i] 0,1 if row i covered by the actual solution */
    /** Note: Use incremental updates for the solution **/
    int fx;           /* sum of the cost of the columns selected in the solution (can be partial) */
    
    /** Dynamic variables **/
    /** Note: use dynamic variables to make easier the construction and modification of solutions.  **/
    /**       these are just examples of useful variables.                                          **/
    /**       these variables need to be updated every time a column is added to a partial solution **/
    /**       or when a complete solution is modified*/
    int used_sets;
    int **col_cover;   /* col_colver[i] selected columns that cover row i */
    int *ncol_cover;   /* number of selected columns that cover row i */
};

extern struct Solution initialize();

struct Solution copy_solution(struct Solution *source) {
    struct Solution new_sol = initialize();
    new_sol.used_sets = source->used_sets;
    new_sol.fx = source->fx;
    
    memcpy(new_sol.x, source->x, n * sizeof(int));
    memcpy(new_sol.y, source->y, m * sizeof(int));
    
    for (int i=0; i<m; i++)
        memcpy(&new_sol.col_cover[i], &source->col_cover[i], ncol[i] * sizeof(int));
        memcpy(new_sol.ncol_cover, source->ncol_cover, m * sizeof(int));
        
        return new_sol;
}

void usage(){
    printf("\nUSAGE: lsscp [param_name, param_value] [options]...\n");
    printf("Parameters:\n");
    printf("  --seed : seed to initialize random number generator\n");
    printf("  --instance: SCP instance to execute.\n");
    printf("  --output: Filename for output results.\n");
    printf("Options:\n");
    printf("  --ch1: random solution construction\n");
    printf("  --ch2: static cost-based greedy values.\n");
    printf("  --ch3: static cover cost-based greedy values.\n");
    printf("  --ch4: Adaptive cover cost-based greedy values.\n");
    printf("  --re: applies redundancy elimination after construction.\n");
    printf("  --bi: best improvement.\n");
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
        } else if (strcmp(argv[i], "--output") == 0) {
            output_file=argv[i+1];
            i+=1;
        } else if (strcmp(argv[i], "--ch1") == 0) {
            ch1=1;
        } else if (strcmp(argv[i], "--ch2") == 0) {
            ch2=1;
        } else if (strcmp(argv[i], "--ch3") == 0) {
            ch3=1;
        } else if (strcmp(argv[i], "--ch4") == 0) {
            ch4=1;
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
    
}

/*** Read instance in the OR-LIBRARY format ***/
void read_scp(char *filename) {
    int h,i,j;
    int *k;
    FILE *fp = fopen(filename, "r" );
    
    if (fscanf(fp,"%d",&m)!=1)   /* number of rows = elements */
        error_reading_file("ERROR: there was an error reading instance file.");
    if (fscanf(fp,"%d",&n)!=1)   /* number of columns  = sets*/
        error_reading_file("ERROR: there was an error reading instance file.");
    
    /* Cost of the n columns */
    cost = (int *) mymalloc(n*sizeof(int));
    for (j=0; j<n; j++)
        if (fscanf(fp,"%d",&cost[j]) !=1)
            error_reading_file("ERROR: there was an error reading instance file.");
    
    /* Info of columns that cover each row */
    col  = (int **) mymalloc(m*sizeof(int *)); //indexes of columns that cover each row
    ncol = (int *) mymalloc(m*sizeof(int)); //nr of columns that cover each row
    for (i=0; i<m; i++) {
        if (fscanf(fp,"%d",&ncol[i])!=1) //First: read nr of sets that cover the element
            error_reading_file("ERROR: there was an error reading instance file.");
        col[i] = (int *) mymalloc(ncol[i]*sizeof(int));
        for (h=0; h<ncol[i]; h++) {
            if( fscanf(fp,"%d",&col[i][h])!=1 )
                error_reading_file("ERROR: there was an error reading instance file.");
            col[i][h]--; //I suppose in the instance file they start indexing at 1...
        }
    }
    
    /* Info of rows that are covered by each column */
    row  = (int **) mymalloc(n*sizeof(int *)); //Indexes of rows that are covered by each column
    nrow = (int *) mymalloc(n*sizeof(int)); //Nr of rows that are covered by each column
    k    = (int *) mymalloc(n*sizeof(int));
    for (j=0; j<n; j++) nrow[j]=0;
    for (i=0; i<m; i++) {
        for (h=0; h<ncol[i]; h++)
            nrow[col[i][h]]++;
    }
    for (j=0; j<n; j++) {
        row[j] = (int *) mymalloc(nrow[j]*sizeof(int));
        k[j]   = 0;
    }
    for (i=0;i<m;i++) {
        for (h=0;h<ncol[i];h++) {
            row[col[i][h]][k[col[i][h]]] = i;
            k[col[i][h]]++;
        }
    }
    free((void *)k);
}

/*** Use level>=1 to print more info (check the correct reading) */
void print_instance(int level){
    int i;
    
    printf("**********************************************\n");
    printf("  SCP INSTANCE: %s\n", scp_file);
    printf("  PROBLEM SIZE\t m = %d\t n = %d\n", m, n);
    
    if(level >=1){
        printf("  COLUMN COST:\n");
        for(i=0; i<n;i++)
            printf("%d ",cost[i]);
        printf("\n\n");
        printf("  NUMBER OF ROWS COVERED BY COLUMN 1 is %d\n", nrow[0] );
        for(i=0; i<nrow[0];i++)
            printf("%d ", row[0][i]);
        printf("\n");
        printf("  NUMBER OF COLUMNS COVERING ROW 1 is %d\n", ncol[0] );
        for(i=0; i<ncol[0];i++)
            printf("%d ", col[0][i]);
        printf("\n");
    }
    
    printf("**********************************************\n\n");
    
}

/*** Use this function to initialize other variables of the algorithms **/
struct Solution initialize(){
    struct Solution sol;
    int i;
    sol.x = (int *) mymalloc(n*sizeof(int));
    for (i = 0; i < n; i++) sol.x[i] = 0;
        sol.y = (int *) mymalloc(m*sizeof(int));
        for (i = 0; i < m; i++) sol.y[i] = 0;
            
            sol.col_cover = (int **) mymalloc(m*sizeof(int));
            for (i=0; i<m; i++)
                sol.col_cover[i] = (int *) mymalloc(ncol[i]*sizeof(int));
                
                sol.ncol_cover = (int *) mymalloc(m*sizeof(int));
                for (i = 0; i<m; i++) sol.ncol_cover[i] = 0;
                    sol.fx = 0;
                    sol.used_sets = 0;
                    return sol;
}

/** Check if some elements aren't covered by a set yet in the current solution **/
int uncovered_elements(struct Solution sol) {
    for (int i = 0; i < m; i++)
        if(!sol.y[i]) return 1;
    return 0;
}

int added_elements(struct Solution sol, int set) {
    int count = 0;
    for (int i = 0; i < nrow[set]; i++)
        if (!sol.y[row[set][i]]) count++;
    return count;
}

/** Choose a set according to the chosen algorithm(s) **/
int choose_set(struct Solution sol, int exclude_set) {
    if (ch1) {
        int found_element = 0;
        int chosen_element = 0;
        while (!found_element) {
            chosen_element = rand() % m;
            if(!sol.y[chosen_element]) found_element = 1;
        }
        return col[chosen_element][rand() % ncol[chosen_element]];
    } else if (ch2 || ch3 || ch4) {
        int best_set = 0;
        float best_cost = -1;
        float current_cost;
        int extra_covered;
        for (int i = 0; i < n; i++) {
            extra_covered = added_elements(sol, i);
            if(extra_covered == 0 || i == exclude_set) continue; //Skip the set if it doesn't cover elements that weren't covered yet
            //Calculate cost according to chosen algorithm
            if(ch2) current_cost = cost[i];
            else if(ch3) current_cost = (float) cost[i] / (float) nrow[i];
            else current_cost = (float) cost[i] / (float) extra_covered;
            
            if (current_cost < best_cost && !sol.x[i] && best_cost >= 0) { //Cost of set we're handling is better than the best one we currently have
                best_set = i;
                best_cost = current_cost;
            } else if (current_cost == best_cost && nrow[i] > nrow[best_set] && !sol.x[i] && best_cost >= 0) { //Cost is equal but new one covers more elements
                best_set = i;
                best_cost = current_cost;
            } else if (!sol.x[i] && (best_cost < 0)) { //Initialize index and cost with the first set that isn't already included (and isn't redundant)
                best_set = i;
                best_cost = current_cost;
            }
        }
        return best_set;
    } else {
        printf("No algorithm passed, choosing randomly\n");
        return rand() % n;
    }
}

/*** Build solution ***/
struct Solution execute(struct Solution sol, int exclude_set) {
    int chosen_set; // Set chosen by algorithm
    while (uncovered_elements(sol)) {
        chosen_set = choose_set(sol, exclude_set);
        for (int i = 0; i < nrow[chosen_set]; i++) { //Say that we cover each element that the chosen set covers
            sol.y[row[chosen_set][i]] = 1;
            sol.col_cover[i][sol.ncol_cover[i]] = chosen_set;
            sol.ncol_cover[i]++;
        }
        sol.used_sets++;
        sol.x[chosen_set] = 1;
        sol.fx += cost[chosen_set];
    }
    return sol;
}

struct Solution remove_set(struct Solution sol, int set) {
    sol.x[set] = 0;
    sol.fx -= cost[set];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < sol.ncol_cover[i]; j++) {
            if(sol.col_cover[i][j] == set) {
                sol.ncol_cover[i]--;
                break;
            }
        }
    }
    return sol;
}

int find_max_weight_set(struct Solution sol, int * exclude_sets) {
    int current_weight;
    int max_weight_set = 0;
    int max_weight = 0;
    for (int i = 0; i < n; i++) {
        current_weight = cost[i];
        if (sol.x[i] && cost[i] > max_weight && !exclude_sets[i]) {
            max_weight_set = i;
            max_weight = current_weight;
}
    }
    return max_weight_set;
}

/*** Remove redundant sets from the solution***/
struct Solution redundancy_elimination(struct Solution sol) {
    int i, max_weight_set, can_remove, covered_by_set;
    int *tried = (int *) mymalloc(n*sizeof(int));
    for (i = 0; i < n; i++) tried[i] = 0;
        int counter = sol.used_sets;
        while (counter) {
            counter--;
            max_weight_set = find_max_weight_set(sol, tried);
            tried[max_weight_set] = 1;
            can_remove = 1;
            covered_by_set = 0;
            for (i = 0; i < m; i++) {
                for (int j = 0; j < sol.ncol_cover[i]; j++) {
                    if(sol.col_cover[i][j] == max_weight_set) {
                        covered_by_set = 1;
                        break;
                    }
                    if (covered_by_set && sol.ncol_cover[i] == 1) {
                        can_remove = 0;
                        break;
                    }
                }
            }
            if (can_remove) {
                sol = remove_set(sol, max_weight_set);
            }
        }
    return sol;
}


/**Working of best_improvement:
 - Find set with i'th highest cost that's used and remove it
 - Use adapted execute function that doesn't use removed set
 - Calculate and save cost
 - keep solution with lowest cost
 **/
struct Solution best_improvement(struct Solution sol) {
    int improvement = 1;
    int max_weight_set;
    int *tried = (int *) mymalloc(n*sizeof(int));
    struct Solution best_solution = copy_solution(&sol);
    while (improvement) {
        improvement = 0;
        int counter = sol.used_sets;
        while (counter) {
            counter--;
            max_weight_set = find_max_weight_set(sol, tried);
            tried[max_weight_set] = 1;
            struct Solution new_sol = copy_solution(&sol);
            new_sol = remove_set(new_sol, max_weight_set);
            new_sol = execute(new_sol, max_weight_set);
            if(new_sol.fx < best_solution.fx) {
//                printf("Found new solution with fx %i instead of %i\n", new_sol.fx, best_solution.fx);
                improvement = 1;
                best_solution = new_sol;
            }
        }
        best_solution = redundancy_elimination(best_solution);
    }
    return best_solution;
}


struct Solution first_improvement(struct Solution sol) {
    int improvement = 1;
    int max_weight_set;
    int *tried = (int *) mymalloc(n*sizeof(int));
    struct Solution best_solution = copy_solution(&sol);
    while (improvement) {
        improvement = 0;
        int counter = sol.used_sets;
        while (counter) {
            counter--;
            max_weight_set = find_max_weight_set(sol, tried);
            tried[max_weight_set] = 1;
            struct Solution new_sol = copy_solution(&sol);
            new_sol = remove_set(new_sol, max_weight_set);
            new_sol = execute(new_sol, max_weight_set);
            if(new_sol.fx < best_solution.fx) {
//                printf("Found new solution with fx %i instead of %i\n", new_sol.fx, best_solution.fx);
                improvement = 1;
                best_solution = new_sol;
                best_solution = redundancy_elimination(best_solution);
                break;
            }
        }
    }
    return best_solution;
}

/*** Use this function to finalize execution */
void finalize(){
    free((void **) row );
    free((void **) col );
    free((void *) nrow );
    free((void *) ncol );
    free((void *) cost );
    //free((void **) col_cover);
    //free((void *) ncol_cover);
}

int main(int argc, char *argv[]) {
    read_parameters(argc, argv);
    srand(seed); /*set seed */
    read_scp(scp_file);
    //print_instance(0);
    struct Solution sol = initialize();
    sol = execute(sol, -1); // index of set to exclude is set to -1: don't exlude a set from possible being used
    if (re) sol = redundancy_elimination(sol);
    if (bi) sol = best_improvement(sol);
    else if (fi) sol = first_improvement(sol);
    printf("%i", sol.fx);
    finalize();
    return EXIT_SUCCESS;
}
