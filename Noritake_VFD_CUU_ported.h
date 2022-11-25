

#ifndef NORITAKE_VFD_CUU_PORTED_H
#define	NORITAKE_VFD_CUU_PORTED_H

#include <stdint.h>
#include <stddef.h>

#include "CUU_M68_4bit_ported.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct Noritake_VFD_CUU_struct {
	//protected:
	uint8_t	display;	// display is on
	uint8_t	cursor;		// cursor is displayed
	uint8_t	blink;		// cursor is blinking
	uint8_t	rightToLeft;	// text direction is right-to-left
	uint8_t	autoscroll;	// autoscroll on input
    uint8_t    hasBrightnessBoost; // module has brightness boost
    uint8_t    is_cu20045_uw4j; // module is a CU20045-UW4J
    CUU_Interface *io; // interface
	
	//public:
    
    int     cols; // Number of columns on the display
    int     lines; // Number of lines on the display.
    uint8_t    bc_font;// true=Japanese (default); false=European
    uint8_t    bc_vfd; // true=DS2045G; false=CU-U
	
} Noritake_VFD_CUU;

	//protected:
	
	void CUU_setDisplay(Noritake_VFD_CUU* vfd);	// Set display according to display/cursor/blink members
	void CUU_setDirection(Noritake_VFD_CUU* vfd);	// Set text direction based on rightToLeft/autoscroll
	void CUU_printNumber_long(Noritake_VFD_CUU* vfd, long number, int _base);
	void CUU_printNumber_ulong(Noritake_VFD_CUU* vfd, unsigned long number, int _base);
	void CUU_nextLine(Noritake_VFD_CUU* vfd);
	
	//public:
    
    void CUU_begin(Noritake_VFD_CUU* vfd, int cols, int lines);
    void CUU_interface(Noritake_VFD_CUU* vfd, CUU_Interface* interface);
    void CUU_brightnessBoost(Noritake_VFD_CUU* vfd);
    void CUU_cu20045_uw4j(Noritake_VFD_CUU* vfd);
    
    void CUU_bcVFD(Noritake_VFD_CUU* vfd);
    void CUU_japaneseFont(Noritake_VFD_CUU* vfd);
    void CUU_europeanFont(Noritake_VFD_CUU* vfd);
    
	/*Noritake_VFD_CUU () {
        display = true; TODO: set to true in init function!!!
		cursor = false;
		blink = false;
		rightToLeft = false;
		autoscroll = false;
        hasBrightnessBoost = false;
        is_cu20045_uw4j = false;
	}*/
	
    int CUU_init(Noritake_VFD_CUU* vfd);
	void CUU_clearScreen(Noritake_VFD_CUU* vfd);
	void CUU_home(Noritake_VFD_CUU* vfd);
	void CUU_setCursor(Noritake_VFD_CUU* vfd, uint8_t pos);
	void CUU_setCursor_2d(Noritake_VFD_CUU* vfd, uint8_t col, uint8_t line);
	void CUU_createChar(Noritake_VFD_CUU* vfd, uint8_t num, uint8_t *data);
	void CUU_readChar(Noritake_VFD_CUU* vfd, uint8_t *data, uint8_t num);
	
	void CUU_command(Noritake_VFD_CUU* vfd, uint8_t data);
	void CUU_writeData(Noritake_VFD_CUU* vfd, uint8_t data);
	uint8_t CUU_readData(Noritake_VFD_CUU* vfd);
	uint8_t CUU_readCommand(Noritake_VFD_CUU* vfd);
	uint8_t CUU_readAddress(Noritake_VFD_CUU* vfd);
	uint8_t CUU_readRAM(Noritake_VFD_CUU* vfd);
	uint8_t CUU_readBusy(Noritake_VFD_CUU* vfd);
	
	void CUU_print_char(Noritake_VFD_CUU* vfd, char data);
	void CUU_print_str(Noritake_VFD_CUU* vfd, const char *str);
	void CUU_print_buf(Noritake_VFD_CUU* vfd, const uint8_t *buffer, size_t size);
	void CUU_print_int(Noritake_VFD_CUU* vfd, int number, int _base);
	void CUU_print_uint(Noritake_VFD_CUU* vfd, unsigned int number, int _base);
	void CUU_print_long(Noritake_VFD_CUU* vfd, long number, int _base);
	void CUU_print_ulong(Noritake_VFD_CUU* vfd, unsigned long number, int _base);
	void CUU_println_char(Noritake_VFD_CUU* vfd, char data);
	void CUU_println_str(Noritake_VFD_CUU* vfd, const char *str);
	void CUU_println_buf(Noritake_VFD_CUU* vfd, const uint8_t *buffer, size_t size);
	void CUU_println_int(Noritake_VFD_CUU* vfd, int number, int _base);
	void CUU_println_uint(Noritake_VFD_CUU* vfd, unsigned int number, int _base);
	void CUU_println_long(Noritake_VFD_CUU* vfd, long number, int _base);
	void CUU_println_ulong(Noritake_VFD_CUU* vfd, unsigned long number, int _base);
	
	void CUU_displayOn(Noritake_VFD_CUU* vfd);
	void CUU_displayOff(Noritake_VFD_CUU* vfd);
	void CUU_cursorOn(Noritake_VFD_CUU* vfd);
	void CUU_cursorOff(Noritake_VFD_CUU* vfd);
	void CUU_blinkOn(Noritake_VFD_CUU* vfd);
	void CUU_blinkOff(Noritake_VFD_CUU* vfd);
	void CUU_scrollDisplayLeft(Noritake_VFD_CUU* vfd);
	void CUU_scrollDisplayRight(Noritake_VFD_CUU* vfd);
	void CUU_leftToRight(Noritake_VFD_CUU* vfd);
	void CUU_rightToLeft(Noritake_VFD_CUU* vfd);
	void CUU_autoscroll(Noritake_VFD_CUU* vfd);
	void CUU_noAutoscroll(Noritake_VFD_CUU* vfd);
	void CUU_brightness(Noritake_VFD_CUU* vfd, int brightness);
    
#ifdef	__cplusplus
}
#endif

#endif	/* NORITAKE_VFD_CUU_PORTED_H */