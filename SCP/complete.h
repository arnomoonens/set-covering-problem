//
//  complete.h
//  SCP
//
// Code to build or complete a solution
//
//  Created by Arno Moonens on 29/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef complete_h
#define complete_h

#include <stdio.h>

#include "instance.h"
#include "solution.h"

void execute(struct Instance *instance, struct Solution *sol, int ch, int exclude_set);

#endif /* complete_h */
