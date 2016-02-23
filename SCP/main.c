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

/** Solution variables **/
int *x;           /* x[i] 0,1 if column i is selected */
int *y;           /* y[i] 0,1 if row i covered by the actual solution */
/** Note: Use incremental updates for the solution **/
int fx;           /* sum of the cost of the columns selected in the solution (can be partial) */

/** Dynamic variables **/
/** Note: use dynamic variables to make easier the construction and modification of solutions.  **/
/**       these are just examples of useful variables.                                          **/
/**       these variables need to be updated every time a column is added to a partial solution **/
/**       or when a complete solution is modified*/
int *col_cover;   /* col_colver[i] selected columns that cover row i */
int ncol_cover;   /* number of selected columns that cover row i */

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
void initialize(){
    int i;
    x = (int *) mymalloc(n*sizeof(int));
    for (i = 0; i < n; i++) x[i] = 0;
    y = (int *) mymalloc(m*sizeof(int));
    for (i = 0; i < m; i++) y[i] = 0;
    
    col_cover = (int *) mymalloc(m*sizeof(int));
    ncol_cover = (int) mymalloc(m*sizeof(int));
    fx = 0;
}

/** Check if some elements aren't covered by a set yet in the current solution **/
int uncovered_elements() {
    for (int i = 0; i < m; i++)
        if(!y[i]) return 1;
    return 0;
}

int added_elements(int set) {
    int count = 0;
    for (int i = 0; i < nrow[set]; i++)
        if (!y[row[set][i]]) count++;
    return count;
}

/** Choose a set according to the chosen algorithm(s) **/
int choose_set() {
    if (ch1) {
        int found_element = 0;
        int chosen_element = 0;
        while (!found_element) {
            chosen_element = rand() % m;
            if(!y[chosen_element]) found_element = 1;
        }
        return col[chosen_element][rand() % ncol[chosen_element]];
    } else if (ch2 || ch3 || ch4) {
        int best_set = 0;
        float best_cost = -1;
        float current_cost;
        int extra_covered;
        for (int i = 0; i < n; i++) {
            extra_covered = added_elements(i);
            if(extra_covered == 0) continue; //Skip the set if it doesn't cover elements that weren't covered yet
            //Calculate cost according to chosen algorithm
            if(ch2) current_cost = cost[i];
            else if(ch3) current_cost = (float) cost[i] /  (float) nrow[i];
            else current_cost = (float) cost[i] /  (float) added_elements(i);
            
            if (current_cost < best_cost && !x[i] && best_cost >= 0) { //Cost of set we're handling is better than the best one we currently have
                best_set = i;
                best_cost = current_cost;
            } else if (current_cost == best_cost && nrow[i] > nrow[best_set] && !x[i] && best_cost >= 0) { //Cost is equal but new one covers more elements
                best_set = i;
                best_cost = current_cost;
            } else if (!x[i] && (best_cost < 0)) { //Initialize index and cost with the first set that isn't already included (and isn't redundant)
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
void execute() {
    int chosen_set; // Set chosen by algorithm
    while (uncovered_elements()) {
        chosen_set = choose_set();
        for (int i = 0; i < nrow[chosen_set]; i++) //Say that we cover each element that the chosen set covers
            y[row[chosen_set][i]] = 1;
        x[chosen_set] = 1;
        fx += cost[chosen_set];
    }
    printf("Done! Resulting cost: %i\n", fx);
}


/*** Use this function to finalize execution */
void finalize(){
    free((void **) row );
    free((void **) col );
    free((void *) nrow );
    free((void *) ncol );
    free((void *) cost );
}

int main(int argc, char *argv[]) {
    read_parameters(argc, argv);
    srand(seed); /*set seed */
    read_scp(scp_file);
    //print_instance(0);
    initialize();
    execute();
    finalize();
    return EXIT_SUCCESS;
}
