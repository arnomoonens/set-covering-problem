//
//  utils.h
//  SCP
//
//  Created by Arno Moonens on 5/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>
#include <math.h>

void *mymalloc( size_t size );
void error_reading_file(char *text);

int random_with_probability(double p);


#endif /* utils_h */
