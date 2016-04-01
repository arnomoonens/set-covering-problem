//
//  aco.h
//  SCP
//
// Ant Colony Optimization
// Based on the paper: "New ideas for applying ant colony optimization to the set covering problem"
//
//  Created by Arno Moonens on 31/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef aco_h
#define aco_h

#include <stdio.h>


#include "utils.h"
#include "instance.h"
#include "solution.h"


void aco_construct(struct Instance *instance, struct Solution *sol, double *pheromones, double *heuristic_factor);

#endif /* aco_h */
