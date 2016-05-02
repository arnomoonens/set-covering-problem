//
//  instance.c
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "instance.h"


/*** Read instance in the OR-LIBRARY format ***/
instance * read_scp(char *filename) {
    int h,i,j;
    int *k;
    instance *inst = (instance *) mymalloc(sizeof(instance));
    FILE *fp = fopen(filename, "r" );

    if (fscanf(fp,"%d",&inst->m)!=1)   /* number of rows = elements */
        error_reading_file("ERROR: there was an error reading instance file.");
    if (fscanf(fp,"%d",&inst->n)!=1)   /* number of columns  = sets*/
        error_reading_file("ERROR: there was an error reading instance file.");

    /* Cost of the n columns */
    inst->cost = (int *) mymalloc(inst->n*sizeof(int));
    for (j=0; j<inst->n; j++)
        if (fscanf(fp,"%d",&inst->cost[j]) !=1)
            error_reading_file("ERROR: there was an error reading instance file.");

    /* Info of columns that cover each row */
    inst->col  = (int **) mymalloc(inst->m*sizeof(int *)); //indexes of columns that cover each row
    inst->ncol = (int *) mymalloc(inst->m*sizeof(int)); //nr of columns that cover each row
    for (i=0; i<inst->m; i++) {
        if (fscanf(fp,"%d",&inst->ncol[i])!=1) //First: read nr of sets that cover the element
            error_reading_file("ERROR: there was an error reading instance file.");
        inst->col[i] = (int *) mymalloc(inst->ncol[i]*sizeof(int));
        for (h=0; h<inst->ncol[i]; h++) {
            if( fscanf(fp,"%d",&inst->col[i][h])!=1 )
                error_reading_file("ERROR: there was an error reading instance file.");
            inst->col[i][h]--; //I suppose in the instance file they start indexing at 1...
        }
    }

    /* Info of rows that are covered by each column */
    inst->row  = (int **) mymalloc(inst->n*sizeof(int *)); //Indexes of rows that are covered by each column
    inst->nrow = (int *) mymalloc(inst->n*sizeof(int)); //Nr of rows that are covered by each column
    k    = (int *) mymalloc(inst->n*sizeof(int));
    for (j=0; j<inst->n; j++) inst->nrow[j]=0;
    for (i=0; i<inst->m; i++) {
        for (h=0; h<inst->ncol[i]; h++)
            inst->nrow[inst->col[i][h]]++;
    }
    for (j=0; j<inst->n; j++) {
        inst->row[j] = (int *) mymalloc(inst->nrow[j]*sizeof(int));
        k[j]   = 0;
    }
    for (i=0;i<inst->m;i++) {
        for (h=0;h<inst->ncol[i];h++) {
            inst->row[inst->col[i][h]][k[inst->col[i][h]]] = i;
            k[inst->col[i][h]]++;
        }
    }
    free((void *)k);
    return inst;
}

/*** Use level>=1 to print more info (check the correct reading) */
void print_instance(instance *inst, int level, char *scp_file) {
    int i;

    printf("**********************************************\n");
    printf("  SCP INSTANCE: %s\n", scp_file);
    printf("  PROBLEM SIZE\t m = %d\t n = %d\n", inst->m, inst->n);

    if(level >=1){
        printf("  COLUMN COST:\n");
        for(i=0; i<inst->n;i++)
            printf("%d ",inst->cost[i]);
        printf("\n\n");
        printf("  NUMBER OF ROWS COVERED BY COLUMN 1 is %d\n", inst->nrow[0] );
        for(i=0; i<inst->nrow[0];i++)
            printf("%d ", inst->row[0][i]);
        printf("\n");
        printf("  NUMBER OF COLUMNS COVERING ROW 1 is %d\n", inst->ncol[0] );
        for(i=0; i<inst->ncol[0];i++)
            printf("%d ", inst->col[0][i]);
        printf("\n");
    }

    printf("**********************************************\n\n");

}

/** Check if set covers element in inst **/
int set_covers_element(instance *inst, int set, int element) {
    int i;
    for (i = 0; i < inst->ncol[element]; i++) {
        if (inst->col[element][i] == set) {
            return 1;
        }
    }
    return 0;
}

/** Set with lowest cost that covers an element **/
int lowest_covering_set(instance *inst, int element) {
    int i = 0, lowest = 0, lowest_c = -1, set, c;
    for (; i < inst->ncol[element]; i++) {
        set = inst->col[element][i];
        c = inst->cost[set];
        if (lowest_c < 0 || c < lowest_c) {
            lowest = set;
            lowest_c = c;
        }
    }
    return lowest;
}