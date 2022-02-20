#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"

void initPWM();

void setPWM(double capacitance);