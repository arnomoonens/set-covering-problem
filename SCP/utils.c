#include "utils.h"


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


//WRITE AS MACRO INSTEAD?
int random_with_probability(double p) {
    return ((double)rand() / (double)RAND_MAX) > p;
}