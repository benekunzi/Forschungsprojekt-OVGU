#include "precisionTimer.h"

void initTimer(){
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_init(TIMER_GROUP_0, TIMER_1, &config);
    timer_start(TIMER_GROUP_0,TIMER_0);
    timer_start(TIMER_GROUP_0, TIMER_1);
}

void printTime(){
    uint64_t timerValue;
    while(1){
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timerValue);
        printf("Counter: 0x%08x%08x\r\n", (uint32_t) (timerValue >> 32),
           (uint32_t) (timerValue));
        printf("Time   : %.8f s\r\n", (double) timerValue / TIMER_SCALE);
    }
}