//
//  improvement.h
//  SCP
//
// Code for best improvement and first improvement
//
//  Created by Arno Moonens on 29/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef improvement_h
#define improvement_h

#include <stdio.h>

#include "instance.h"
#include "solution.h"
#include "complete.h"

void best_improvement(instance *inst, solution **sol, void (*notify_improvement)(solution *));
void first_improvement(instance *inst, solution **sol, void (*notify_improvement)(solution *));

#endif /* improvement_h */
