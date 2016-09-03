// Copyright (c) 2016 Quenio Cesar Machado dos Santos. All rights reserved.

#include "timer.h"

double elapsed_secs(clock_t start)
{
    const double t = (double) (clock() - start);
    return (t / CLOCKS_PER_SEC);
}
