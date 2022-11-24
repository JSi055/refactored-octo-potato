
#include "Fake_Arduino.h"


void pinMode(uint8_t pin, uint8_t mode) {
    volatile uint16_t* reg = pin & 0x10 ? &TRISB : &TRISA;
    if (mode == OUTPUT) {
        // put a 0 in TRISx at the pin location
        *reg &= ~(1 << (pin & 0x0F));
    } else {
        // put a 1 in TRISx at the pin location
        *reg |= (1 << (pin & 0x0F));
    }
}

void digitalWrite(uint8_t pin, uint8_t val) {
    volatile uint16_t* reg = pin & 0x10 ? &LATB : &LATA;

    if (val == LOW) {
        // put a 0 in LATx at the pin location
        *reg &= ~(1 << (pin & 0x0F));
    } else {
        // put a 1 in LATx at the pin location
        *reg |= (1 << (pin & 0x0F));
    }
}

int digitalRead(uint8_t pin) {
    volatile uint16_t* reg = pin & 0x10 ? &PORTB : &PORTA;
    // get the bit in PORTx at the pin location
    return (*reg >> (pin & 0x0F)) & 1;
}