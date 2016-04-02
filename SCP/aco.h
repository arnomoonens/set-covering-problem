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
#include <time.h>
#include <limits.h>

#include "utils.h"
#include "instance.h"
#include "solution.h"

struct Solution * aco_execute(struct Instance *instance, double maxtime, int nants, double beta, double ro, double epsilon);

#endif /* aco_h */
