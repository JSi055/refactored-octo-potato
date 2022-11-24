//#include "Arduino.h"
#include <xc.h>
#include <stdint.h>
//#include <util/delay.h>

#define DIRECTION(X,D)  if (D) pinMode(inter->X##_PIN, OUTPUT); else pinMode(inter->X##_PIN, INPUT)
#define RAISE(X)	digitalWrite(inter->X##_PIN, HIGH)
#define LOWER(X)	digitalWrite(inter->X##_PIN, LOW)
#define CHECK(X)	digitalRead(inter->X##_PIN)
#define SETPIN(X,V) digitalWrite(inter->X##_PIN, (V)? HIGH: LOW)

//class CUU_Interface {

typedef struct CUU_Interface_struct {
    uint8_t RS_PIN;
    uint8_t RW_PIN;
    uint8_t E_PIN;
    uint8_t D4_PIN;
    uint8_t D5_PIN;
    uint8_t D6_PIN;
    uint8_t D7_PIN;
} CUU_Interface;

//public:
void CUU_init(CUU_Interface *inter);

void write(CUU_Interface *inter, uint8_t data, char rs) = 0;
uint8_t read(char rs);
//    virtual bool is8bit() = 0;
//};



/*class CUU_Parallel_M68_4bit : public CUU_Interface {
protected:
    unsigned RS_PIN:4;
    unsigned RW_PIN:4;
    unsigned E_PIN:4;
    unsigned D4_PIN:4;
    unsigned D5_PIN:4;
    unsigned D6_PIN:4;
    unsigned D7_PIN:4;

public:
    CUU_Parallel_M68_4bit(uint8_t rs, uint8_t wr, uint8_t rd,
      uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7):
        RS_PIN(rs), RW_PIN(wr), E_PIN(rd),
        D4_PIN(d4), D5_PIN(d5), D6_PIN(d6), D7_PIN(d7)
    {
    }*/
    
void init(CUU_Interface *inter) {
    RAISE(RW);
        
    DIRECTION(RS, 1);
    DIRECTION(RW, 1);
    DIRECTION(E, 1);
    
    DIRECTION(D4, 1);
    DIRECTION(D5, 1);
    DIRECTION(D6, 1);
    DIRECTION(D7, 1);    
        
    write(0x33, 0);      // 8-bit function set, 8-bit function set
    write(0x32, 0);      // 8-bit function set, 4-bit function set
}

void write(uint8_t data, char rs) {    
        SETPIN(D4, data & 0x10);
        SETPIN(D5, data & 0x20);
        SETPIN(D6, data & 0x40);
        SETPIN(D7, data & 0x80);
        
        SETPIN(RS, rs);
        LOWER(RW);
        
        RAISE(E);
        _delay_us(0.5);
        LOWER(E);
        
        SETPIN(D4, data & 0x01);
        SETPIN(D5, data & 0x02);
        SETPIN(D6, data & 0x04);
        SETPIN(D7, data & 0x08);
        RAISE(E);
        _delay_us(0.5);
        LOWER(E);
    }
    uint8_t read(bool rs) {
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