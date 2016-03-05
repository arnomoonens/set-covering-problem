#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

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

