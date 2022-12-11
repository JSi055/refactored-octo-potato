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
#include "Input_handler_Functions.h"

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

// with division: 3950 cycles with -O2 or -O3
// with division: 4350 cycles with -O0
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


void __attribute__((interrupt, auto_psv)) _U1RXInterrupt() {
    _U1RXIF = 0;   
    
    
   // TODO: Read the first byte to know the packet type. Start building
    // the packet. Once the packet is complete, process it with its handler function.
    // 
    //struct { char type; char (*handler_ptr)(char* buffer, char len) }
    //The handler will read the current buffer and return the number of bytes
    // to buffer before the next call. If bit 7 is set, the buffer is consumed.
    // If the return value is 0, the packet is complete and the next byte is another
    // command.
    
    input_buffer[front++] = U1RXREG;
    if(front >= 1000)                  //adds new byte to circular buffer
        front = 0;
    
    if(input_buffer[front-1] ){
        inputbuffer[x]=curbyte
        x++;
    }
    else
        go to switch
                
    switch(input_buffer[read]) {   //determine if char is sent    
        case 's':   //print status string
            s_ptr{input_buffer[],}; 
            break;
        case 'T':   //test pulse
            T_ptr{input_buffer[],}; 
            break;
        case 'f':    //voltage stream on/off
            f_ptr(){input_buffer[],}; 
            break;
        case 'D':    //dump data
            D_ptr(){input_buffer[],}; 
            break;
        case 'v':    //voltage stream packets
            v_ptr(){input_buffer[],}; 
            break;
        case 't':    //conduct test
            t_ptr(){input_buffer[],}; 
            break;
        case 'c':   //calibrate voltage or current
            c_ptr(){input_buffer[],}; 
            break;
         case 'l':   //set load in micro amps
            l_ptr(){input_buffer[],}; 
            break;   
    }
    
    // simple command reader    
    while (U1STAbits.URXDA) { // while there is stuff in the RX buffer...
        TRISAbits.TRISA4 = 0; // 100 Ohm
        TRISBbits.TRISB4 = 0; // 500 Ohm
        TRISAbits.TRISA3 = 0; // 2000 Ohm
        uint8_t val = U1RXREG;
        U1TXREG = val; // ECHO TEST
        switch(val) {
            case '1': // 100 Ohm
                LATAbits.LATA4 = 1;
                break;
            case '2': // 500 Ohm
                LATBbits.LATB4 = 1;
                break;
            case '3': // 2000 Ohm
                LATAbits.LATA3 = 1;
                break;
            case '4': // 100 Ohm off
                LATAbits.LATA4 = 0;
                break;
            case '5': // 500 Ohm off
                LATBbits.LATB4 = 0;
                break;
            case '6': // 2000 Ohm off
                LATAbits.LATA3 = 0;
                break;
            default: // unknown command
                break;
        }
    }
}

void __attribute__((interrupt, auto_psv)) _U1TXInterrupt() {
    
}
    // TODO: If data needs transmitting, take it out of the queue
    // and put it in the TX buffer. The queue should block if it is full,
    // so the main process may be waiting on us.
    
    // TODO: The queue put function should set _U1TXIF so we don't freeze
    // TODO: If data needs streaming, put it in the TX buffer
    
    
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
    // By default it is 1 (devide clock by 2). 0 is divide by 1.
    CLKDIVbits.RCDIV = 0b000;  //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    
    setup_debug();
 
    // Make all pins digital accept for the battery voltage sensing pin
    AD1PCFG = 0x9FFF & ~(1 << BAT_VOLTAGE_PIN);    
    
    // ================ Digital Pins ================
    //                  (Test Loads)
    
    //LATBbits.LATB6 = 0; //
    //LATBbits.LATB7 = 0; // default
    //LATBbits.LATB8 = 0; //
    
    // Set RA4, RB4, and RA3 as Outputs
    TRISAbits.TRISA4 = 0; // 100 Ohm
    TRISBbits.TRISB4 = 0; // 500 Ohm
    TRISAbits.TRISA3 = 0; // 2000 Ohm
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
    RPINR18bits.U1RXR = 2; // RP2 ===> U1RX
    //RPINR18bits.U1CTSR =
    RPOR1bits.RP3R = 3;    // U1TX ===> RP3. note: '3' is the U1TX number, not pin
    //RPOR bits.RP R = 3;
    __builtin_write_OSCCONL(OSCCON | 0x40);
    
    U1MODE = 0;
    U1BRG = 34;              //34 = baud = 115200 TODO: try faster speeds if it works
    U1MODEbits.BRGH = 1;      // fast mode (divide by 4 instead of 16 for U1BRG)
    U1MODEbits.PDSEL = 0b00;  // no parity. TODO: use software parity or hardware
    U1MODEbits.STSEL = 0;     // one stop bit
    // Interrupt when RX buffer is 3/4 full (0b10) caused the last parts of
    // some commands to sit in the buffer until the next command was sent.
    // Maybe we could fix this by triggering the interrupt when the receiver
    // has gone idle (I know there is a bit for that somewhere).
    U1STAbits.URXISEL = 0b00; // Interrupt every byte
    U1STAbits.UTXISEL1 = 1; // 0b10 Interrupt when TX buffer is empty
    U1STAbits.UTXISEL0 = 0; // ^^^^
    
    _U1TXIF = 0;
    _U1RXIF = 0;
    IEC0bits.U1TXIE = 1; //optional  interrupts
    IEC0bits.U1RXIE = 1;

    U1MODEbits.UARTEN = 1;  // enable U1 UART
    U1STAbits.UTXEN = 1;    // enable transmit
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
    
    //Setup Buffer
    char input_Buffer[1000] = 0;
    int buffer_pointer = 0;
    //end setup Buffer
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
        
        //protoype A/D to UART. Not final position 
        //possible implementation with array for multiple voltages?
        
        uint8_t voltage = ADC1BUF0; //6 bit 0, 10 bit a/d values
        voltage = (voltage*points)/5610;  //this is a linear scaling of points to voltage for now
                                          //need more info on points for greater detail
                                          //also decimal points not fully accounted for yet
        U1TXREG or quene[] = voltage; //depends if direct send or put into quene for interrupt here
        
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