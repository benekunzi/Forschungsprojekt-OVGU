#include "pwm.h"

void initPWM(){
    ledc_timer_config_t timerConfig = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0, 
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timerConfig);

    ledc_channel_config_t channelConfig = {
        .gpio_num = 16,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
    };
    ledc_channel_config(&channelConfig);
}

void setPWM(double capacitance){
    int duty = 0;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    while(1){
        for(int i=0; i<8192; i+=10){
            
            vTaskDelay(10/ portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}