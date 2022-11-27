
#include "running_average.h"

void exp_mov_create(average_moving_exponential* moving, uint16_t setpoint) {
    moving->sum = 0;
    moving->count = 0;
    moving->setpoint = setpoint;
}

uint16_t exp_mov_poll(average_moving_exponential* moving, uint8_t overPrecision) {
    return moving->count ? (moving->sum * overPrecision) / moving->count : 0;
}

avg_fetch exp_mov_fetch(average_moving_exponential* moving, uint8_t overPrecision) {
    uint16_t purity = moving->purity;
    moving->purity = 0;
    return (avg_fetch){
        .val = exp_mov_poll(moving, overPrecision),
        .purity = purity
    };
}

void exp_mov_updateSetpoint(average_moving_exponential* moving, uint16_t setpoint) {
    if (setpoint < moving->count) {
        // re-scale sum and count by slicing off a number of averages 
        moving->sum -= (moving->sum / moving->count) * (moving->count - setpoint);
        moving->count = setpoint;
    } // else { do nothing, put handles the increase like normal }
    moving->setpoint = setpoint;
}