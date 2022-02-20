#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "driver/timer.h"

#define TIMER_DIVIDER         (2)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // convert counter value to seconds

void initTimer();

void printTime();