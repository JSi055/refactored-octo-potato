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
    // The number of values that should make up about 63% of the average.
    // Our 'time constant' so to speak.
    uint16_t setpoint;
    // The humber of values we have received sense the last fetch operation.
    uint16_t purity;
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
void exp_mov_create(volatile average_moving_exponential* moving, uint16_t setpoint);

/**
 * Add a value to the moving average.
 */
void exp_mov_put(volatile average_moving_exponential* moving, uint16_t value);

/**
 * Get the average without resetting purity.
 * An overPrecision > 1 allows more precision to be squeezed out than the
 * values put into the average.
 */
uint16_t exp_mov_poll(volatile average_moving_exponential* moving, uint8_t overPrecision);

/**
 * Fetch the current average value and purity.
 * An overPrecision > 1 allows more precision to be squeezed out than the
 * values put into the average.
 */
avg_fetch exp_mov_fetch(volatile average_moving_exponential* moving, uint8_t overPrecision);

/**
 * Update's the setpoint
 */
void exp_mov_updateSetpoint(volatile average_moving_exponential* moving, uint16_t setpoint);

#ifdef	__cplusplus
}
#endif

#endif	/* RUNNING_AVERAGE_H */

