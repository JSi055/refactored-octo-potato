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
#include "calibration_Functions.h"

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

#define BAT_VOLTAGE_PIN 10 // AN10

// Keeps a running average of the battery voltage. Maybe we can squeeze another
// 2 bits of precision by averaging some noise.
// note: we might need just the right amount of noise.
// Removing volatile saved some cycles and it still seems to work.
average_moving_exponential vbat_avg;

CUU_Interface screen_interface;
Noritake_VFD_CUU screen;
volatile uint16_t debug_profile;

// no test is being done
// normal current with CURRENT_CLUTCH_PIN low
#define TEST_IDLE          0

// Hold CURRENT_CLUTCH_PIN high,
// Set PWM duty cycle up to test level!
// (warm up the capacitor to ready the test current)
// about 1s
#define TEST_WARMUP        1

// Capacitor is ready.
// ======> capture some pre test voltage values.
#define TEST_CAPTURE_PRE   2

// Bring CURRENT_CLUTCH_PIN back to low to fire the current!
// ======> capture test values!
#define TEST_CAPTURE_PULSE 3

// Hold CURRENT_CLUTCH_PIN high,
// ======> capture post test values!
#define TEST_CAPTURE_POST  4

// Hold CURRENT_CLUTCH_PIN high,
// Set PWM duty cycle back to normal
// (allow capacitor to get back to normal)
#define TEST_COOLDOWN      5

#define CURRENT_CLUTCH_PIN LATAbits.LATA2
#define CURRENT_PWM_PIN 4 // RP4 - note: hard coded elsewere

#define PURITY_THRESHOLD 128

// ##### TEST PULSE #####
volatile char test_phase = TEST_IDLE;
volatile uint32_t test_dur_us;
volatile uint32_t test_pre_dur_us;
volatile uint32_t test_post_dur_us;
volatile uint16_t test_current_duty = 0;

volatile uint16_t test_time;
volatile uint16_t test_time_until; // save some multiplication ops

#define TEST_BUFFER_SHIFT 9 // 1 << 9 = 512
#define TEST_BUFFER_MASK ((1 << TEST_BUFFER_SHIFT) - 1)
#define TEST_BUFFER_SIZE (1 << TEST_BUFFER_SHIFT)
volatile uint16_t test_buffer[TEST_BUFFER_SIZE];
volatile uint16_t test_buf_front = 0;
volatile uint16_t test_buf_back = 0;

// ##### Discharge Curve & Status #####
#define CFG_STREAM_VOLTAGES = 0x01
#define CFG_STREAM_AUTOMATIC = 0x02
volatile uint16_t run_current_duty = 0;
volatile uint16_t last_purity = 0;
volatile uint8_t config_flags = 0;

// ##### UART #####
typedef int (*handler_ptr)(char*, int);

#define INPUT_BUFFER_SIZE 64
volatile char input_buffer[INPUT_BUFFER_SIZE];
volatile uint8_t input_count = 0;
volatile uint8_t input_count_required = 0;
volatile handler_ptr current_handler;

#define OUTPUT_BUFFER_SHIFT 6 // 1 << 6 = 64
#define OUTPUT_BUFFER_MASK ((1 << OUTPUT_BUFFER_SHIFT) - 1)
#define OUTPUT_BUFFER_SIZE (1 << OUTPUT_BUFFER_SHIFT)
volatile char output_buffer[OUTPUT_BUFFER_SIZE];
volatile uint8_t output_buf_front = 0;
volatile uint8_t output_buf_back = 0;
volatile uint8_t output_buf_lock = 0;

void uart_transmit(char* data, int len);
void setCurrent(int duty);

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
        exp_mov_put(&vbat_avg, (&ADC1BUF0)[i]);
    }
#if PURITY_THRESHOLD != 0
    if (vbat_avg.purity > PURITY_THRESHOLD) {
        _U1TXIF = 1; // TODO: this is just a quick fix
    }
#endif
    debug_profile = TMR1 - debug_profile; // DEBUG
}

void putTestVoltage() {
    avg_fetch vtest = exp_mov_fetch(&vbat_avg);
    last_purity = vtest.purity;
    test_buffer[test_buf_front++] = vtest.val; // FIXME
    test_buf_front &= TEST_BUFFER_MASK; // avoid % to save cycles
    if (test_buf_front == test_buf_back) {
        // o no! buffer overflow!
        uart_transmit("eBuffer Overflow!\n", 18);
    }
    
    // if there was a flag to see if the transmit buffer was empty.
    // we would use it. U1STAbits.UTXBF just says there is one space
    // (almost always true, so save some cycles by not checking)
    _U1TXIF = 1; // kick start the transmitter. 
}

// test phase interrupt. TODO: priorities
void __attribute__((interrupt, auto_psv)) _T4Interrupt() {
    _T4IF = 0;
    switch (test_phase) {
        case TEST_WARMUP:
            test_phase = TEST_CAPTURE_PRE;
            T4CONbits.TON = 0;
            T4CONbits.TCKPS = 0b10; // 64x pre-scale, 4us at 16M clock
            TMR4 = 0;
            // sample every vbat_avg setpoint has passed
            // PR4 = 140 * 64 / 64
            PR4 = ((uint32_t)PR3 << vbat_avg.setpoint) >> 6;
            test_time = 0;
            test_time_until = (test_pre_dur_us >> 2) / PR4;
            
            T4CONbits.TON = 1;
            
            break;
        case TEST_CAPTURE_PRE:
            putTestVoltage();
            
            if (++test_time > test_time_until) {
                test_phase = TEST_CAPTURE_PULSE;
                test_time = 0;
                test_time_until = (test_dur_us >> 2) / PR4;
                CURRENT_CLUTCH_PIN = 0; // DO THE PULSE!
            }
            break;
        case TEST_CAPTURE_PULSE:
            putTestVoltage();
            
            if (++test_time > test_time_until) {
                CURRENT_CLUTCH_PIN = 1; // END THE PULSE!
                test_phase = TEST_CAPTURE_POST;
                test_time = 0;
                test_time_until = (test_post_dur_us >> 2) / PR4;
            }
            break;
        case TEST_CAPTURE_POST:
            putTestVoltage();
            
            if (++test_time > test_time_until) {
                test_phase = TEST_COOLDOWN;
                T4CONbits.TON = 0;
                
                setCurrent(run_current_duty);
                
                T4CONbits.TCKPS = 0b11; // 256x pre-scale
                PR4 = 62500; // 1 second cooldown
                TMR4 = 0;
                T4CONbits.TON = 1;
            }
            break;
        case TEST_COOLDOWN:
            T4CONbits.TON = 0;
            _T1IE = 0;
            
            CURRENT_CLUTCH_PIN = 0; // return to normal discharge curve
            uart_transmit("T", 1);
            
            test_phase = TEST_IDLE;
            break;
        default:
            uart_transmit("e1\n", 3);
    }
}


void startTest() {
    CURRENT_CLUTCH_PIN = 1;
    setCurrent(test_current_duty);
    test_phase = TEST_WARMUP;
    
    T4CON = 0;
    _T4IF = 0;
    _T4IE = 1;
    T4CONbits.TCKPS = 0b11; // 256x pre-scale
    PR4 = 62500; // 1 second warmup
    TMR4 = 0;
    T4CONbits.TON = 1;
}

/**
 * Sets the current (effective when CURRENT_CLUTCH_PIN is low)
 * @param duty 0 to MAX_CURRENT
 */
void setCurrent(int duty) {
    // the voltage divider is 1k / (1k + 20k)
    // that voltage will be made across a 100 ohm resistor to create the current
    OC1RS = duty;
}

uint32_t bigbytesToInt32(uint8_t* data) {
    return (uint32_t)data[0] << 24
         | (uint32_t)data[1] << 16
         | (uint32_t)data[2] << 8
         | (uint32_t)data[3] << 0;
}

uint16_t bigbytesToInt16(uint8_t* data) {
    return (uint16_t)data[0] << 8
         | (uint16_t)data[1] << 0;
}

int handle_test(char* data, int len){
    // wait until we have enough bytes
    if (len < 14) {
        return 14;
    }
    
    if(test_phase == TEST_IDLE) {
        // use network byte order
        test_dur_us     = bigbytesToInt32(&data[0]);
        test_pre_dur_us = bigbytesToInt32(&data[4]);
        test_post_dur_us = bigbytesToInt32(&data[8]);
        test_current_duty = bigbytesToInt16(&data[12]);
        
        startTest();
    }

    return CMD_END;
}

int handle_status(char* data, int len){
    uart_transmit("sTODO: status\n", 14);
    
    return CMD_END;
}

int handle_load(char* data, int len) {
    // wait until we have enough bytes
    if (len < 2) {
        return 2;
    }
    
    run_current_duty = bigbytesToInt16(&data[0]);
    if (test_phase == TEST_IDLE) {
        setCurrent(run_current_duty);
    }
    return CMD_END;
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
    //TRISAbits.TRISA4 = 0; // 100 Ohm
    //TRISBbits.TRISB4 = 0; // 500 Ohm
    //TRISAbits.TRISA3 = 0; // 2000 Ohm
    // ============== end Digital Pins ==============
    
    
    // ================ Setup ADC ================
    //           (Battery Voltage Sensing)
    // TODO: Current Sensing
    
    // initialize average thingys
    exp_mov_create(&vbat_avg, 6, 2); // 1 << 6 = 64, 2 bits overprecission
    
    AD1CON1 = 0;
    AD1CON1bits.SSRC = 0b010;    // Timer3 - convert on compare
    AD1CON1bits.ASAM = 1;        // Turn on auto-sample after conversion.
    // AD1CON1bits.FORM = 0b00;  // integer format (0 - 1023)
    
    AD1CON2bits.CSCNA = 1; // Turn on channel scanning (ignore CH0SA)
    AD1CSSL = 1 << BAT_VOLTAGE_PIN; // right now we only scan through one pin
    //AD1CHSbits.CH0NA = BAT_VOLTAGE_PIN; // could use this instead
    
    AD1CON2bits.SMPI = (8-1); // interrupt after 8 conversion cycle(s)
    AD1CON2bits.BUFM = 1;     // split the buffers into 2x8 and use BUFS bit to check
    //AD1CON2bits.VCFG = 0b011; // External reference voltages (trying this to reduce noise)
    
    // sample and convert cycles = (12 + SAMC) * ADCS, if SAMC is used (ITS NOT)
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
    
    
    // ============== Setup Current PWM ==============
    // note: our time constant 1ms (16000 cycles)
    // using 256 cycles, e^(-(256*62.5*10^-9)/0.001) = 0.984
    // Get Timer2 ready, but don't start it yet.
    T2CON = 0;   // clear T3CON (should be clear by default)
    T2CONbits.TCKPS = 0; // 1x pre-scale
    PR2 = 256;
    
    CURRENT_CLUTCH_PIN = 0;
    TRISAbits.TRISA2 = 0;   // Switch current clutch pin to output mode
    
    // Setup Output Compare 1
    
    // clear OC1CON (clear by default, but lets do it anyway)
    OC1CON = 0;
    
    // MUST DO THIS WHILE OCM is still 000!!! Else, OC1R will be frozen and
    // does not even copy from OC1RS! (found this out the hard way)
    // use setServo() to set OC1RS for us
    setCurrent(0);
    OC1R = OC1RS;
    
    OC1CONbits.OCTSEL = 0;      // Tell OC1 to use T2 timer. 1 would be T3.
    OC1CONbits.OCM    = 0b110;  // Set OC1 to PWM mode
    
    // Connect RP OC1
    __builtin_write_OSCCONL(OSCCON & 0xBF); // unlock
    RPOR2bits.RP4R = 18;                    // OC1 (id 18) ===> RP4
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock
    
    T2CONbits.TON = 1;          // Enable the T2 timer!
    // ============ end Setup Current PWM ============
    
    
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
    U1BRG = 34;               // 34 = baud = 115200 TODO: try faster speeds if it works
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
    IEC0bits.U1TXIE = 1; // enable interrupts
    _U1TXIP = 5; // give TX a higher priority so we don't have a race condition
    IEC0bits.U1RXIE = 1;

    U1MODEbits.UARTEN = 1;  // enable U1 UART
    U1STAbits.UTXEN = 1;    // enable transmit
    // ============== end Setup UART ==============
    
    
    // ================ Setup VFD Screen ================
    //  (Initialize the Noritake itron CU16025ECPB-U5J
    //   VFD display. Note: this setup uses delays)
    waitMS(250); // wait for the screen to power on
    
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
    
    while(1){
        /*LATBbits.LATB6 = 1;
        LATBbits.LATB6 = 0;
        LATBbits.LATB7 = 1;
        LATBbits.LATB7 = 0;
        LATBbits.LATB8 = 1;
        LATBbits.LATB8 = 0;*/
        
        //waitMS(20);
        
        /* Scrap code
        // CALIBRATION POINTS (at Tom's home, Nov 26, with BM786):
        // 5610 ==> 3.2880v
        
        //protoype A/D to UART. Not final position 
        //possible implementation with array for multiple voltages?
        
        uint8_t voltage = ADC1BUF0; //6 bit 0, 10 bit a/d values
        voltage = (voltage*points)/5610;  //this is a linear scaling of points to voltage for now
                                          //need more info on points for greater detail
                                          //also decimal points not fully accounted for yet
        U1TXREG or quene[] = voltage; //depends if direct send or put into quene for interrupt here
        */
        
        CUU_setCursor_2d(&screen, 0, 0);
        CUU_print_str(&screen, "v: ");
        CUU_print_uint(&screen, exp_mov_poll(&vbat_avg), 10);
        
        CUU_print_str(&screen, " p: ");
        CUU_print_uint(&screen, last_purity, 10);
        CUU_print_str(&screen, "  ");
        
        CUU_setCursor_2d(&screen, 0, 1);
        CUU_print_str(&screen, "DBG: ");
        CUU_print_uint(&screen, debug_profile, 10);
        CUU_print_str(&screen, "  ");
    }
}

// ####### \/ UART STUFF DOWN HERE \/ ########

int handle_unknown(char* data, int len) {
    uart_transmit("ec\n", 3);
    return CMD_END;
}

int handle_ping(char* data, int len) {
    uart_transmit("p", 1);
    return CMD_END;
}

handler_ptr uart_get_handler(char cmd_id) {
    switch (cmd_id) {
        case 's':
            return handle_status;
        case 'p':
            return handle_ping;
        case 'T':
            return handle_test;
        case 'l':
            return handle_load;
        default:
            return handle_unknown;
    }
}

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt() {
    _U1RXIF = 0;
    
    while (U1STAbits.URXDA) { // while there is stuff in the RX buffer...
        uint8_t val = U1RXREG;
        
        if (current_handler) {
            input_buffer[input_count++] = val;
        } else {
            current_handler = uart_get_handler(val);
            input_count = 0;
            input_count_required = 0;
        }
        
        if (input_count >= input_count_required) {
            int result = current_handler(input_buffer, input_count);
            if (result > 0) {
                // keep buffering
                input_count_required = result;
            } else {
                if (result < 0) {
                    input_count_required = -result;
                } else {
                    current_handler = NULL;
                }
                // buffer consumed
                input_count = 0;
            }
        }
    }
}

void uart_transmit(char* data, int len) {
    output_buf_lock = 1;
    for (int i = 0; i < len; i++) {
        output_buffer[output_buf_front++] = data[i];
        output_buf_front &= OUTPUT_BUFFER_MASK;
        if (output_buf_front == output_buf_back) {
            uart_transmit("et\n", 3);
        }
    }
    output_buf_lock = 0;
    // extra check out here incase a race _U1TXInterrupt() had to abort
    if (U1STAbits.UTXBF) _U1TXIF = 1;
}

// TX buffer is empty according to uart setup
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt() {
    _U1TXIF = 0;
    
    while (!U1STAbits.UTXBF) {
        if (output_buf_back != output_buf_front) {
            // take from the buffer
            U1TXREG = output_buffer[output_buf_back++];
            output_buf_back &= OUTPUT_BUFFER_MASK;
        } else {
            // we empty! dump some data into the buffer and re-loop!
            if (output_buf_lock || vbat_avg.purity < PURITY_THRESHOLD) {
                // TODO: quick fix remove this check ^
                return; // race condition! abort!
            }
            
            uint16_t bigEndian;
            if (test_buf_back != test_buf_front) {
                U1TXREG = 't'; // put it in the buffer early to buy some time
                bigEndian = test_buffer[test_buf_back++];
                test_buf_back &= TEST_BUFFER_MASK;
                bigEndian = (bigEndian >> 8) | (bigEndian << 8);
                uart_transmit((char*) &bigEndian, 2);
            } else if (test_phase == TEST_IDLE) {
                U1TXREG = 'v'; // put = exp_mov_fetch(&vbat_avg); it in the buffer early to buy some time
                avg_fetch avg = exp_mov_fetch(&vbat_avg); 
                bigEndian = avg.val;
                last_purity = avg.purity;
                bigEndian = (bigEndian >> 8) | (bigEndian << 8);
                uart_transmit((char*) &bigEndian, 2);
            } else {
                // nothing to put on the buffer!
                break;
            }
        }
    }
}