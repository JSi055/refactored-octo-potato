#include "CUU_M68_4bit_ported.h"
#include <xc.h>
#include <stdint.h>
#include "lonne025_delays_v002.h"

#define DIRECTION(X,D)  if (D) pinMode(inter->X##_PIN, OUTPUT); else pinMode(inter->X##_PIN, INPUT)
#define RAISE(X)	digitalWrite(inter->X##_PIN, HIGH)
#define LOWER(X)	digitalWrite(inter->X##_PIN, LOW)
#define CHECK(X)	digitalRead(inter->X##_PIN)
#define SETPIN(X,V) digitalWrite(inter->X##_PIN, (V)? HIGH: LOW)

void CUU_M68_4_create(CUU_Interface *inter) {
    inter->init = CUU_M68_4_init;
    inter->write = CUU_M68_4_write;
    inter->read = CUU_M68_4_read;
}

void CUU_M68_4_init(CUU_Interface *inter) {
    RAISE(RW);
        
    DIRECTION(RS, 1);
    DIRECTION(RW, 1);
    DIRECTION(E, 1);
    
    DIRECTION(D4, 1);
    DIRECTION(D5, 1);
    DIRECTION(D6, 1);
    DIRECTION(D7, 1);    
        
    inter->write(inter, 0x33, 0);      // 8-bit function set, 8-bit function set
    inter->write(inter, 0x32, 0);      // 8-bit function set, 4-bit function set
}

void CUU_M68_4_write(CUU_Interface *inter, uint8_t data, char rs) {    
        SETPIN(D4, data & 0x10);
        SETPIN(D5, data & 0x20);
        SETPIN(D6, data & 0x40);
        SETPIN(D7, data & 0x80);
        
        SETPIN(RS, rs);
        LOWER(RW);
        
        RAISE(E);
        _delay_half_us();
        LOWER(E);
        
        SETPIN(D4, data & 0x01);
        SETPIN(D5, data & 0x02);
        SETPIN(D6, data & 0x04);
        SETPIN(D7, data & 0x08);
        RAISE(E);
        _delay_half_us();
        LOWER(E);
    }

uint8_t CUU_M68_4_read(CUU_Interface *inter, char rs) {
        DIRECTION(D4, 0);
        DIRECTION(D5, 0);
        DIRECTION(D6, 0);
        DIRECTION(D7, 0);
        
        SETPIN(RS, rs);    
        RAISE(RW);
        
        RAISE(E);
        _delay_us(5);
        uint8_t data =
            CHECK(D7) << 7 |
            CHECK(D6) << 6 |
            CHECK(D5) << 5 |
            CHECK(D4) << 4;
        LOWER(E);
        _delay_us(5);
        
        RAISE(E);
        _delay_us(5);
        data |=
            CHECK(D7) << 3 |
            CHECK(D6) << 2 |
            CHECK(D5) << 1 |
            CHECK(D4);
        LOWER(E);
        
        DIRECTION(D4, 1);
        DIRECTION(D5, 1);
        DIRECTION(D6, 1);
        DIRECTION(D7, 1);
        
        return data;
    }
//    bool is8bit() { return false; }
//};