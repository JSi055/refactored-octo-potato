/* 
 * File:   Input_handler_Functions.h
 * Author: Jake's pc
 *
 * Created on December 10, 2022, 2:43 PM
 */

#ifndef INPUT_HANDLER_FUNCTIONS_H
#define	INPUT_HANDLER_FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define CMD_END 0
#define CMD_CONSUMED_BUF -1

// scrap code
/*
struct cmd {char cmd_type; 
            int (*handler_ptr)(char* buffer, int len); };

void (*s_ptr)(char input_buffer[], int length) = status_handler; 
void (*T_ptr)(char input_buffer[], int length) = pulse_handler;
void (*f_ptr)(char input_buffer[], int length) = toggle_handler;
void (*D_ptr)(char input_buffer[], int length) = dump_handler;
void (*v_ptr)(char input_buffer[], int length) = stream_handler;
void (*t_ptr)(char input_buffer[], int length) = test_handler;
void (*c_ptr)(char input_buffer[], int length) = calibration_handler;
void (*l_ptr)(char input_buffer[], int length) = load_handler;
*/

#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_HANDLER_FUNCTIONS_H */

