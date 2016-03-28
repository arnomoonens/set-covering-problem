//
//  instance.c
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#include "instance.h"


/*** Read instance in the OR-LIBRARY format ***/
struct Instance * read_scp(char *filename) {
    int h,i,j;
    int *k;
    struct Instance *instance = (struct Instance *) mymalloc(sizeof(struct Instance));
    FILE *fp = fopen(filename, "r" );
    
    if (fscanf(fp,"%d",&instance->m)!=1)   /* number of rows = elements */
        error_reading_file("ERROR: there was an error reading instance file.");
    if (fscanf(fp,"%d",&instance->n)!=1)   /* number of columns  = sets*/
        error_reading_file("ERROR: there was an error reading instance file.");
    
    /* Cost of the n columns */
    instance->cost = (int *) mymalloc(instance->n*sizeof(int));
    for (j=0; j<instance->n; j++)
        if (fscanf(fp,"%d",&instance->cost[j]) !=1)
            error_reading_file("ERROR: there was an error reading instance file.");
    
    /* Info of columns that cover each row */
    instance->col  = (int **) mymalloc(instance->m*sizeof(int *)); //indexes of columns that cover each row
    instance->ncol = (int *) mymalloc(instance->m*sizeof(int)); //nr of columns that cover each row
    for (i=0; i<instance->m; i++) {
        if (fscanf(fp,"%d",&instance->ncol[i])!=1) //First: read nr of sets that cover the element
            error_reading_file("ERROR: there was an error reading instance file.");
        instance->col[i] = (int *) mymalloc(instance->ncol[i]*sizeof(int));
        for (h=0; h<instance->ncol[i]; h++) {
            if( fscanf(fp,"%d",&instance->col[i][h])!=1 )
                error_reading_file("ERROR: there was an error reading instance file.");
            instance->col[i][h]--; //I suppose in the instance file they start indexing at 1...
        }
    }
    
    /* Info of rows that are covered by each column */
    instance->row  = (int **) mymalloc(instance->n*sizeof(int *)); //Indexes of rows that are covered by each column
    instance->nrow = (int *) mymalloc(instance->n*sizeof(int)); //Nr of rows that are covered by each column
    k    = (int *) mymalloc(instance->n*sizeof(int));
    for (j=0; j<instance->n; j++) instance->nrow[j]=0;
    for (i=0; i<instance->m; i++) {
        for (h=0; h<instance->ncol[i]; h++)
            instance->nrow[instance->col[i][h]]++;
    }
    for (j=0; j<instance->n; j++) {
        instance->row[j] = (int *) mymalloc(instance->nrow[j]*sizeof(int));
        k[j]   = 0;
    }
    for (i=0;i<instance->m;i++) {
        for (h=0;h<instance->ncol[i];h++) {
            instance->row[instance->col[i][h]][k[instance->col[i][h]]] = i;
            k[instance->col[i][h]]++;
        }
    }
    free((void *)k);
    return instance;
}

/*** Use level>=1 to print more info (check the correct reading) */
void print_instance(struct Instance *instance, int level, char *scp_file) {
    int i;
    
    printf("**********************************************\n");
    printf("  SCP INSTANCE: %s\n", scp_file);
    printf("  PROBLEM SIZE\t m = %d\t n = %d\n", instance->m, instance->n);
    
    if(level >=1){
        printf("  COLUMN COST:\n");
        for(i=0; i<instance->n;i++)
            printf("%d ",instance->cost[i]);
        printf("\n\n");
        printf("  NUMBER OF ROWS COVERED BY COLUMN 1 is %d\n", instance->nrow[0] );
        for(i=0; i<instance->nrow[0];i++)
            printf("%d ", instance->row[0][i]);
        printf("\n");
        printf("  NUMBER OF COLUMNS COVERING ROW 1 is %d\n", instance->ncol[0] );
        for(i=0; i<instance->ncol[0];i++)
            printf("%d ", instance->col[0][i]);
        printf("\n");
    }
    
    printf("**********************************************\n\n");
    
}