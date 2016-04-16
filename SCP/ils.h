//
//  ils.h
//  SCP
//
//  Created by Arno Moonens on 28/03/16.
//  Copyright © 2016 Arno Moonens. All rights reserved.
//

#ifndef ils_h
#define ils_h

#include <stdio.h>
#include <time.h>

#include "instance.h"
#include "solution.h"
#include "utils.h"

void ils_execute(instance *inst, solution **sol, int (*termination_criterion)(solution *), void (*notify_improvement)(solution *), double T, double TL, double CF, double ro1, double ro2);

#endif /* ils_h */
