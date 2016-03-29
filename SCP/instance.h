//
//  instance.h
//  SCP
//
// Code for building and printing an instance
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef instance_h
#define instance_h

#include <stdio.h>

#include "utils.h"

/** Instance static variables **/
struct Instance {
    int m;            /* number of rows = elements */
    int n;            /* number of columns = sets */
    int **row;        /* row[i] rows that are covered by column i */
    int **col;        /* col[i] columns that cover row i */
    int *ncol;        /* ncol[i] number of columns that cover row i */
    int *nrow;        /* nrow[i] number of rows that are covered by column i */
    int *cost;        /* cost[i] cost of column i  */
    int *sorted_by_weight; /* sets sorted by weight (in descending order) */
};

struct Instance * read_scp(char *filename);
void print_instance(struct Instance *instance, int level, char *scp_file);

#endif /* instance_h */
