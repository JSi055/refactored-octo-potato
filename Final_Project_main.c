/*
 * File:   Final_Project_main.c
 * Author: Thomas Lonneman and Jacob Simons
 *
 * Created on November 22, 2022, 11:57 AM
 */


#include <p24FJ64GA002.h>
#include "xc.h"

//#include "Final_Project_AsmLib.h"

#include "Noritake_VFD_CUU_ported.h"

#include "running_average.h"

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


#define BAT_VOLTAGE_PIN 9 // AN9
//test

// Keeps a running average of the battery voltage. Maybe we can squeeze another
// 2 bits of precision by averaging some noise.
// TODO: we might need just the right amount of noise.
volatile average_moving_exponential vbat_avg;

Noritake_VFD_CUU screen;

void setup() {
 
    // Make all pins digital accept for the battery voltage sensing pin
    AD1PCFG = 0x9FFF & ~(1 << BAT_VOLTAGE_PIN);    
    
    // ================ Digital Pins ================
    //                  (Test Loads)
    
    //LATBbits.LATB6 = 0; //
    //LATBbits.LATB7 = 0; // default
    //LATBbits.LATB8 = 0; //
    
    TRISBbits.TRISB6 = 0; // Set RB6, RB7, and RB8 as Outputs
    TRISBbits.TRISB7 = 0; //
    TRISBbits.TRISB8 = 0; //
    // ============== end Digital Pins ==============
    
    
    // ================ Setup ADC ================
    //           (Battery Voltage Sensing)
    // TODO: Current Sensing
    AD1CON1 = 0;
    AD1CON1bits.SSRC = 0b111;    // Internal counter (auto-convert)
    AD1CON1bits.ASAM = 1;        // Turn on auto-sample after conversion.
    // AD1CON1bits.FORM = 0b00;  // integer format (0 - 1023)
    
    AD1CON2bits.CSCNA = 1; // Turn on channel scanning (ignore CH0SA)
    AD1CSSL = 1 << BAT_VOLTAGE_PIN; // cycle through AN0 through AN4
    //AD1CHSbits.CH0NA = BAT_VOLTAGE_PIN;
    
    AD1CON2bits.SMPI = (1-1); // interrupt after 1 conversion cycle(s)
    AD1CON2bits.BUFM = 1;     // split the buffers into 2x8 and use BUFS bit to check
    
    // sample and convert cycles = (12 + SAMC) * ADCS = (12 + 10) * 2 = 44
    AD1CON3bits.ADCS = (2-1); // 2 x Tcy = 125ns
    // If our resistor network is 10k Ohm, and the cap is 4.4pF, the RC
    // time constant is 0.704 cycles.
    // e^-(0.704*t) = 1024, t = 9.85 cycles. Lets double that for safety.
    AD1CON3bits.SAMC = 10;
    
    _AD1IF = 0; // clear IF flag
    _AD1IP = 4; // set priority 4 (default) TODO: compare to other interrupts
    _AD1IE = 1; // enable interrupts 
    
    AD1CON1bits.ADON = 1; // start the analog to digital converter
    // ============== end Setup ADC ==============
    
    
    // ================ Setup UART ================
    // (115200 baud TTL to send data, and get commands from a PC)
    // TODO: pick the pins!
    
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
    // ============== end Setup UART ==============
    
    
    // ================ Setup VFD Screen ================
    //  (Initialize the Noritake itron CU16025ECPB-U5J
    //   VFD display. Note: this setup uses delays)
    
    
    
    // ============== end Setup VFD Screen ==============
}

int main() {    //main will need all setup functions, write UARTS, and sending signals 
    setup();
    while(1){   
        LATBbits.LATB6 = 1;
        LATBbits.LATB6 = 0;
        LATBbits.LATB7 = 1;
        LATBbits.LATB7 = 0;
        LATBbits.LATB8 = 1;
        LATBbits.LATB8 = 0;
    }
}