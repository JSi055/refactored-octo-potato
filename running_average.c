
#include "running_average.h"

void exp_mov_create(volatile average_moving_exponential* moving, uint16_t setpoint) {
    moving->sum = 0;
    moving->count = 0;
    moving->setpoint = setpoint;
}

void exp_mov_put(volatile average_moving_exponential* moving, uint16_t value) {
    if (moving->count < moving->setpoint) {
        // accumulate more values until the setpoint is reached
        moving->count++;
    } else {
        // remove a value from the sum to make space for the new one
        moving->sum -= moving->sum / moving->count;
    }
    // add 1 to purity or stay at max
    // a purity higher than the setpoint
    // uniqueness = 1 - e^-(purity/count)
    // when purity == setpoint, we have roughly %63 uniqueness
    moving->purity += moving->purity == 0xFFFF ? 0 : 1;
    // add the new value to the sum
    moving->sum += value;
}

uint16_t exp_mov_poll(volatile average_moving_exponential* moving, uint8_t overPrecision) {
    return moving->count ? (moving->sum * overPrecision) / moving->count : 0;
}

avg_fetch exp_mov_fetch(volatile average_moving_exponential* moving, uint8_t overPrecision) {
    uint16_t purity = moving->purity;
    moving->purity = 0;
    return (avg_fetch){
        .val = exp_mov_poll(moving, overPrecision),
        .purity = purity
    };
}

void exp_mov_updateSetpoint(volatile average_moving_exponential* moving, uint16_t setpoint) {
    if (setpoint < moving->count) {
        // re-scale sum and count by slicing off a number of averages 
        moving->sum -= (moving->sum / moving->count) * (moving->count - setpoint);
        moving->count = setpoint;
    } // else { do nothing, put handles the increase like normal }
    moving->setpoint = setpoint;
}