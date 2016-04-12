#include "utils.h"

/** Allocate memory of a certain size **/
void *mymalloc( size_t size ) {
    void *s;
    if ( (s=malloc(size)) == NULL ) {
        fprintf( stderr, "malloc : Not enough memory.\n" );
        exit( EXIT_FAILURE );
    }
    return s;
}

/** print error when file can't be read **/
void error_reading_file(char *text){
    printf("%s\n", text);
    exit( EXIT_FAILURE );
}


//WRITE AS MACRO INSTEAD?
int random_with_probability(double p) {
    return ((double)rand() / (double)RAND_MAX) < p;
}

int random_with_pdf(double *probabilities, int n) {
    int i;
    double p =(double)rand() / (double)RAND_MAX;
    double cumulative_probability = 0;
    for (i = 0; i < n; i++) {
        cumulative_probability += probabilities[i];
        if (p <= cumulative_probability) {
            return i;
        }
    }
    printf("This should not happen");
    return i;
}