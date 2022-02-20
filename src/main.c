#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_task_wdt.h"
#include "precisionTimer.h"
#include "median_filter.h"

double resistor_CM = 1.0007; //value in MOhm
double resistor_CREF = 0.9966; //value in MOhm
double CM = 0.0;
double CREF = 0.0;
double CREF_median = 0.0;

volatile bool flag_C1 = false;
volatile bool flag_C2 = false;


void IRAM_ATTR cap1_isr(void *args){
    flag_C1 = true;
}

void IRAM_ATTR cap2_isr(void *args){
    flag_C2 = true;
}

void setupPin(){
    //Output Pins 
    gpio_config_t gpio32Config;
    gpio32Config.pin_bit_mask = (1ULL << 32);
    gpio32Config.mode = GPIO_MODE_OUTPUT;
    gpio32Config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio32Config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&gpio32Config);

    gpio_config_t gpio33Config;
    gpio33Config.pin_bit_mask = (1ULL << 33);
    gpio33Config.mode = GPIO_MODE_OUTPUT;
    gpio33Config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio33Config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&gpio33Config);


    //Input Pins
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_0db); //Pin 34
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_0db); //Pin 35

    gpio_config_t gpio34config = {
        .pin_bit_mask = (1ULL << 34),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE, 
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio34config);

    gpio_config_t gpio35config = {
        .pin_bit_mask = (1ULL << 35),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE, 
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio35config);

    gpio_config_t gpio25Config = {
        .pin_bit_mask = (1<<25),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&gpio25Config);
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(25, cap1_isr, NULL);

    gpio_config_t gpio26Config = {
        .pin_bit_mask = (1<<26),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&gpio26Config);
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_isr_handler_add(26, cap2_isr, NULL);
}

void precisMeasurement(){
    uint64_t startticks_CM;
    uint64_t endticks_CM; 
    uint64_t startticks_CREF;
    uint64_t endticks_CREF; 
    uint64_t risingTicks_C1;
    uint64_t risingTicks_C2;
    double risingTime_C1;
    double risingTime_C2;
    int analogValue;

    for(;;){
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &startticks_CM);
        gpio_set_level(GPIO_NUM_32, 1);
        while(!flag_C1){}
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &endticks_CM);
        risingTicks_C1 = (endticks_CM-startticks_CM);           //ticks from register
        risingTime_C1 = risingTicks_C1*0.025;                   //time in μs 
        CM = risingTime_C1/resistor_CM;                         //C in μF

        gpio_set_level(GPIO_NUM_32, 0);
        do{
            analogValue = adc1_get_raw(ADC1_CHANNEL_6);
        }while(analogValue > 0);


        timer_get_counter_value(TIMER_GROUP_0, TIMER_1, &startticks_CREF);
        gpio_set_level(GPIO_NUM_33, 1);
        while(!flag_C2){}
        timer_get_counter_value(TIMER_GROUP_0, TIMER_1, &endticks_CREF);
        risingTicks_C2 = (endticks_CREF-startticks_CREF);       //ticks from register
        risingTime_C2 = risingTicks_C2*0.025;                   //time in μs 
        CREF = risingTime_C2/resistor_CREF;                     //C in μF

        gpio_set_level(GPIO_NUM_33, 0);
        do{
            analogValue = adc1_get_raw(ADC1_CHANNEL_7);
        }while(analogValue > 0);

        flag_C1 = false;
        flag_C2 = false;

        CREF_median = median_filter(CREF);
        printf("%.5f, %.5f\n", CREF, CREF_median);

        vTaskDelay(2/portTICK_PERIOD_MS);
    }   
    vTaskDelete(NULL);
}

void app_main() {
    setupPin();
    initTimer();
    xTaskCreate(precisMeasurement, "precise", 2048, NULL, 0, NULL);
}