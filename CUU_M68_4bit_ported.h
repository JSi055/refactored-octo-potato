/* 
 * File:   CUU_M68_4bit_ported.h
 * Author: xcube
 *
 * Created on November 23, 2022, 9:17 PM
 */

#ifndef CUU_M68_4BIT_PORTED_H
#define	CUU_M68_4BIT_PORTED_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "Fake_Arduino.h"
//class CUU_Interface {

//typedef struct CUU_Interface_struct CUU_Interface;
typedef struct CUU_Interface_struct {
    void (*write)(struct CUU_Interface_struct*, uint8_t, char);
    uint8_t (*read)(struct CUU_Interface_struct*, char);
    uint8_t RS_PIN;
    uint8_t RW_PIN;
    uint8_t E_PIN;
    uint8_t D4_PIN;
    uint8_t D5_PIN;
    uint8_t D6_PIN;
    uint8_t D7_PIN;
} CUU_Interface;

//public:
void CUU_M68_4_init(CUU_Interface *inter);

void CUU_M68_4_write(CUU_Interface *inter, uint8_t data, char rs);
uint8_t CUU_M68_4_read(CUU_Interface *inter, char rs);
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
    


#ifdef	__cplusplus
}
#endif

#endif	/* CUU_M68_4BIT_PORTED_H */

