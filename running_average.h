/* 
 * File:   running_average.c
 * Author: Thomas Lonneman
 *
 * Created on November 25, 2022, 10:25 AM
 */

#ifndef RUNNING_AVERAGE_H
#define	RUNNING_AVERAGE_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * A moving exponential average
 */
typedef struct exp_mov_average_struct {
    // Stores a running sum that exponentially decays/grows in the direction
    // of new values.
    uint32_t sum;
    // Stones the number of values tracked by the sum. This value stops going up
    // when the setpoint is reached.
    uint16_t count;
    // The 2^setpoint number of values that should make up about 63% of the average.
    // Our 'time constant' so to speak.
    uint16_t setpoint;
    // The humber of values we have received sense the last fetch operation.
    uint16_t purity;
    
    uint16_t anti_flicker_val;
    uint8_t overPrecision;
} average_moving_exponential;

/**
 * Because C does not have tuples.
 */
typedef struct avg_fetch_struct {
    uint16_t val;
    uint16_t purity;
} avg_fetch;

/**
 * Initializes or resets a average_moving_exponential.
 * 
 * @param moving The average_moving_exponential to initialize
 * @param setpoint The number of values that make up about 63% of the average.
 */
void exp_mov_create(average_moving_exponential* moving, uint16_t setpoint, uint8_t overPrecision);

/**
 * Add a value to the moving average.
 */
static inline void exp_mov_put(average_moving_exponential* moving, uint16_t value) {
    if (moving->count < 1 << moving->setpoint) {
        // accumulate more values until the setpoint is reached
        moving->count++;
    } else {
        // remove a value from the sum to make space for the new one
        moving->sum -= moving->sum >> moving->setpoint;
    }
    // add 1 to purity or stay at max
    // a purity higher than the setpoint
    // uniqueness = 1 - e^-(purity/count)
    // when purity == setpoint, we have roughly %63 uniqueness
    moving->purity += moving->purity == 0xFFFF ? 0 : 1;
    // add the new value to the sum
    moving->sum += value;
}

/**
 * Get the average without resetting purity.
 * An overPrecision > 1 allows more precision to be squeezed out than the
 * values put into the average.
 */
uint16_t exp_mov_poll(average_moving_exponential* moving);

/**
 * Fetch the current average value and purity.
 * An overPrecision > 1 allows more precision to be squeezed out than the
 * values put into the average.
 */
avg_fetch exp_mov_fetch(average_moving_exponential* moving);

/**
 * Update's the setpoint
 */
void exp_mov_updateSetpoint(average_moving_exponential* moving, uint16_t setpoint);

#ifdef	__cplusplus
}
#endif

#endif	/* RUNNING_AVERAGE_H */

