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
#include "lonne025_delays_v002.h"
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
// note: we might need just the right amount of noise.
// Removing volatile saved some cycles and it still seems to work.
average_moving_exponential vbat_avg;

CUU_Interface screen_interface;
Noritake_VFD_CUU screen;
volatile uint16_t debug_profile;

// 3950 cycles with -O2 or -O3
// 4350 cycles with -O0
void __attribute__((interrupt, auto_psv)) _ADC1Interrupt() {
    debug_profile = TMR1; // DEBUG
    _AD1IF = 0; // clear IF flag
    
    // 1 = A/D is currently filling ADC1BUF8-ADC1BUFF,
    //         user should access data in ADC1BUF0-ADC1BUF7
    // 0 = A/D is currently filling ADC1BUF0-ADC1BUF7,
    //         user should access data in ADC1BUF8-ADC1BUFF
    int offset = AD1CON2bits.BUFS ? 0 : 8;
    for (int i = offset; i < offset + 8; i++) {
        // ADC1BUF (0 through 15) are consecutive in memory
        
        if (vbat_avg.count < 1 << vbat_avg.setpoint) {
            // accumulate more values until the setpoint is reached
            vbat_avg.count++;
        } else {
            // remove a value from the sum to make space for the new one
            vbat_avg.sum -= vbat_avg.sum >> vbat_avg.setpoint;
        }
        // add 1 to purity or stay at max
        // a purity higher than the setpoint
        // uniqueness = 1 - e^-(purity/count)
        // when purity == setpoint, we have roughly %63 uniqueness
        vbat_avg.purity += vbat_avg.purity == 0xFFFF ? 0 : 1;
        // add the new value to the sum
        vbat_avg.sum += (&ADC1BUF0)[i];
    }
    debug_profile = TMR1 - debug_profile; // DEBUG
}

void setup_debug() {
    // We have _ADC1Interrupt taking too long! profile it!
    // ======= DEBUG TIMER (Timer3) ======
    T1CON = 0;
    TMR1 = 0;
    
    PR1 = 0xFFFF;
    T1CONbits.TCKPS = 0b00; // 1 pre-scale
    
    T1CONbits.TON = 1; // turn on the timber
}

void setup() {
    setup_debug();
 
    // Make all pins digital accept for the battery voltage sensing pin
    AD1PCFG = 0x9FFF & ~(1 << BAT_VOLTAGE_PIN);    
    
    // ================ Digital Pins ================
    //                  (Test Loads)
    
    //LATBbits.LATB6 = 0; //
    //LATBbits.LATB7 = 0; // default
    //LATBbits.LATB8 = 0; //
    
    TRISAbits.TRISA4 = 0; // Set RA4, RB4, and RA3 as Outputs
    TRISBbits.TRISB4 = 0; //
    TRISAbits.TRISA3 = 0; //
    // ============== end Digital Pins ==============
    
    
    // ================ Setup ADC ================
    //           (Battery Voltage Sensing)
    // TODO: Current Sensing
    
    // initialize average thingys
    exp_mov_create(&vbat_avg, 7); // 1 << 5 = 64
    
    AD1CON1 = 0;
    AD1CON1bits.SSRC = 0b010;    // Timer3 - sample on compare
    AD1CON1bits.ASAM = 1;        // Turn on auto-sample after conversion.
    // AD1CON1bits.FORM = 0b00;  // integer format (0 - 1023)
    
    AD1CON2bits.CSCNA = 1; // Turn on channel scanning (ignore CH0SA)
    AD1CSSL = 1 << BAT_VOLTAGE_PIN; // cycle through AN0 through AN4
    //AD1CHSbits.CH0NA = BAT_VOLTAGE_PIN;
    
    AD1CON2bits.SMPI = (8-1); // interrupt after 8 conversion cycle(s)
    AD1CON2bits.BUFM = 1;     // split the buffers into 2x8 and use BUFS bit to check
    //AD1CON2bits.VCFG = 0b011; // External reference voltages (trying this to reduce noise)
    
    // sample and convert cycles = (12 + SAMC) * ADCS = (12 + 10) * 2 = 44
    AD1CON3bits.ADCS = (3-1); // 3 x Tcy = 187.2ns
    //AD1CON3bits.SAMC = 31; // Using Timer3 instead
    
    T3CON = 0;
    TMR3 = 0;
    
    // FIXME: The _ADC1Interrupt takes too long to process the values!
    // If our resistor network is 110k Ohm, and the cap is 4.4pF, the RC
    // time constant is R * C * (1/16M) cycles.
    // e^-(t/(4.4*10^-12 * 110*10^3 * 16000000))=1/1024, t = 53.7 cycles.
    PR3 = 140; // convert time takes up (ADCS+1) * 12 = 36
    T3CONbits.TCKPS = 0b00; // 1 pre-scale
    
    _AD1IF = 0; // clear IF flag
    _AD1IP = 4; // set priority 4 (default) TODO: compare to other interrupts
    _AD1IE = 1; // enable interrupts
    
    T3CONbits.TON = 1; // turn on the timber
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
    
    CUU_M68_4_create(&screen_interface);
    screen_interface.RS_PIN = 0x15;
    screen_interface.RW_PIN = 0x16;
    screen_interface.E_PIN  = 0x17;
    screen_interface.D4_PIN = 0x18;
    screen_interface.D5_PIN = 0x19;
    screen_interface.D6_PIN = 0x1A;
    screen_interface.D7_PIN = 0x1B;
    
    CUU_begin(&screen, 16, 2);    // 16x2 character module
    CUU_interface(&screen, &screen_interface); // select which interface to use

    CUU_init(&screen);          // initialize module
    CUU_europeanFont(&screen);  // select European font for DS2045G - I dont think this does anything for the U5J (spec says that bit is itnored)

    CUU_setCursor_2d(&screen, 1, 1);
    CUU_print_str(&screen, "Hello World");
    
    // ============== end Setup VFD Screen ==============
}

int main() {    //main will need all setup functions, write UARTS, and sending signals 
    setup();
    
    uint16_t anti_flicker_last_val = 0;
    
    while(1){
        /*LATBbits.LATB6 = 1;
        LATBbits.LATB6 = 0;
        LATBbits.LATB7 = 1;
        LATBbits.LATB7 = 0;
        LATBbits.LATB8 = 1;
        LATBbits.LATB8 = 0;*/
        
        //waitMS(20);
        
        // CALIBRATION POINTS (at Tom's home, Nov 26, with BM786):
        // 5610 ==> 3.2880v
        
        
        CUU_setCursor_2d(&screen, 0, 0);
        CUU_print_str(&screen, "v: ");
        avg_fetch avg = exp_mov_fetch(&vbat_avg, 8);
        int16_t diff = anti_flicker_last_val - avg.val;
        if (diff > 1 || diff < -1) {
            anti_flicker_last_val = avg.val;
        }
        
        CUU_print_uint(&screen, anti_flicker_last_val >> 1, 10);
        CUU_print_str(&screen, " p: ");
        CUU_print_uint(&screen, avg.purity, 10);
        CUU_print_str(&screen, "  ");
        
        CUU_setCursor_2d(&screen, 0, 1);
        CUU_print_str(&screen, "DBG: ");
        CUU_print_uint(&screen, debug_profile, 10);
        CUU_print_str(&screen, "  ");
    }
}