/*
 * File:   Final_Project_main.c
 * Author: Jake's pc
 *
 * Created on November 22, 2022, 11:57 AM
 */


#include <p24FJ64GA002.h>
#include "Final_Project_AsmLib.h"

#include "xc.h"

#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))


void input_Setup(void);
void test_Output(void);
void UART_Setup(void);
//to do  display function

int main(void) {
    test_Output();
    input_Setup();
    while(1){
        LATBbits.LATB6 = 1;
        LATBbits.LATB6 = 0;
        LATBbits.LATB7 = 1;
        LATBbits.LATB7 = 0;
        LATBbits.LATB8 = 1;
        LATBbits.LATB8 = 0;
        
    }
}

void input_Setup(void){
    AD1PCFG = 0xFDFF;
    AD1CON1 = 0x00E4; //SSRC = 111, ASAM = 1, 
    AD1CON2 = 0x0;    
    AD1CON3 = 0x1F01; //ADCS = 1, SAMC = 111
    AD1CHS = 0x0009;  //CH0SA = 9
//    IFS0bits.AD1IF = 0; //optional interrupt
//    IEC0bits.AD1IE = 1;
    AD1CON1bits.ADON = 1;
}

void test_Output(void){
    TRISBbits.TRISB6 = 0;  //set outputs
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB6 = 0;
    LATBbits.LATB7 = 0;
    LATBbits.LATB8 = 0;
}

void UART_Setup(void){
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    //RPINR18bits.U1RXR =
    //RPINR18bits.U1CTSR =
    //RPOR bits.RP R = 3;
    //RPOR bits.RP R = 3;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    
    U1BRG = 99;     //baud = 10000, temporary
    U1MODE = 0000;  //PDSEL = 00, STSEL = 0; BRGH = 0, UEN = 0;
//    IEC0bits.U1TXIE = 1; //optional  interrupts
//    IEC0bits.U1RXIE = 1;
    U1MODEbits.UARTEN = 1;
    U1STA = 0x400;  //UXTEN = 1; 
}

