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

/** print error when file can't be written to **/
void error_writing_file(char *text) {
    printf("%s\n", text);
    exit( EXIT_FAILURE );
}

/** Returns 1 or 0 by probability p **/
int random_with_probability(double p) {
    return ((double)rand() / (double)RAND_MAX) < p;
}

/** Choose an index by a corresponding array of probabilities **/
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
    printf("ERROR: Nothing chosen using probability distribution.\n");
    exit( EXIT_FAILURE );
}

/** Difference between start and end in seconds **/
double mdifftime(struct timeval *end, struct timeval *start) {
    return (end->tv_sec - start->tv_sec) + (end->tv_usec - start->tv_usec)/(double)1000000;
}