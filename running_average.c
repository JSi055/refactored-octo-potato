
#include "running_average.h"

void exp_mov_create(average_moving_exponential* moving, uint16_t setpoint, uint8_t overPrecision) {
    moving->sum = 0;
    moving->count = 0;
    moving->setpoint = setpoint;
    moving->anti_flicker_val = 0;
    moving->overPrecision = overPrecision + 1; // add 1 for anti-flicker
}

uint16_t exp_mov_poll(average_moving_exponential* moving) {
    if (!moving->count) {
        return 0;
    }
    int16_t new_val;
    if (moving->count != (1 << moving->setpoint)) {
        // use slow division
        new_val = (moving->sum << moving->overPrecision) / moving->count;
    } else {
        // use fast shift
        new_val = (moving->sum << moving->overPrecision) >> moving->setpoint;
    }
    int16_t diff = moving->anti_flicker_val - new_val;
    if (diff > 1 || diff < -1) {
        moving->anti_flicker_val = new_val;
    }
    return moving->anti_flicker_val >> 1;
}

avg_fetch exp_mov_fetch(average_moving_exponential* moving) {
    uint16_t purity = moving->purity;
    moving->purity = 0;
    return (avg_fetch){
        .val = exp_mov_poll(moving),
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