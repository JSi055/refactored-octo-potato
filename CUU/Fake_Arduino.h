/* 
 * File:   Fake_Arduino.h
 * Author: xcube
 *
 * Created on November 23, 2022, 8:56 PM
 */

#ifndef FAKE_ARDUINO_H
#define	FAKE_ARDUINO_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

/**
 * RA is 0 through 15, RB is 16 through 31
 * @param pin
 * @param mode
 */
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);


#ifdef	__cplusplus
}
#endif

#endif	/* FAKE_ARDUINO_H */

