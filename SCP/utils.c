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

