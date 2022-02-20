#include "median_filter.h"

double median_array[10] = {0};

double median_filter(double value){
    size_t size = sizeof(median_array)/sizeof(median_array[0]);
    double sum = 0.0;
    //left shift
    for(register int8_t i=0; i<size-1;i++){
        median_array[i] = median_array[i+1];
    }
    median_array[size-1] = value;
    
    //sum of the array
    for(register int8_t i=0; i<size;i++){
        sum += median_array[i];
    }
    return sum/(double)size;
}