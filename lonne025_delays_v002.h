/* 
 * File:   lonne025_lab2_core_v001.h
 * Author: Thomas Lonneman <lonne025@umn.edu>
 *
 * Created on September 21, 2022, 9:06 PM
 */

#ifndef LONNE025_LAB2_CORE_V001_H
#define	LONNE025_LAB2_CORE_V001_H

#ifdef	__cplusplus
extern "C" {
#endif


/**
 * This function takes exactly 100us to complete.
 */
void wait_100us();

/**
 * This function takes exactly 1ms to complete.
 */
void wait_1ms();

/**
 * DO NOT USE 0 FOR ms!
 * Wait a number of at least one milliseconds
 * @param ms
 */
void waitMS(uint16_t ms);


void _delay_half_us();

/**
 * DO NOT USE 0 FOR us!
 * @param us
 */
void _delay_us(uint16_t us);

#ifdef	__cplusplus
}
#endif

#endif	/* LONNE025_LAB2_CORE_V001_H */