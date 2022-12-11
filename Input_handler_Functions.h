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
void (*s_ptr)(uint8_t curByte)= status_handler; 
void (*T_ptr)(uint8_t curByte)= pulse_handler;
void (*f_ptr)(uint8_t curByte)= toggle_handler;
void (*D_ptr)(uint8_t curByte)= dump_handler;
void (*v_ptr)(uint8_t curByte)= stream_handler;
void (*t_ptr)(uint8_t curByte)= test_handler;
void (*c_ptr)(uint8_t curByte)= calibration_handler;
void (*l_ptr)(uint8_t curByte)= load_handler;


#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_HANDLER_FUNCTIONS_H */

