//#include <avr/io.h>
#include <stddef.h>

#include "CUU_M68_4bit_ported.h"
#include "Noritake_VFD_CUU_ported.h"
#include "lonne025_delays_v002.h"
//#include <util/delay.h>

#define MIN_DELAY 5

    void brightnessBoost(Noritake_VFD_CUU* this) { this->hasBrightnessBoost = true; }
    void cu20045_uw4j(Noritake_VFD_CUU* this) {  this->is_cu20045_uw4j = true; };
    
    void bcVFD(Noritake_VFD_CUU* this) { this->bc_vfd = true; }
    void japaneseFont(Noritake_VFD_CUU* this) { this->bc_font = true; CUU_home(this); }
    void europeanFont(Noritake_VFD_CUU* this) { this->bc_font = false; CUU_home(this); }

void begin(Noritake_VFD_CUU* this, int cols, int lines) {
    this->cols = cols;
    this->lines = lines;
}

void interface(Noritake_VFD_CUU* this, CUU_Interface* interface) {
    this->io = interface;
}

// ----------------------------------------------------------------
// Initialize the VFD module. This must be called before any other
// methods.
int
CUU_init(Noritake_VFD_CUU* this) {
    this->io->init(this->io);
    CUU_brightness(this, 100);
	CUU_displayOn(this);
	CUU_leftToRight(this);
	CUU_clearScreen(this);
	return 0;
}

//----------------------------------------------------------------
//void CUU_command(uint8_t data)
//Send command to the VFD controller.
//data: command to send
void
CUU_command(Noritake_VFD_CUU* this, uint8_t data) {
	this->io->write(this->io, data, false);
	_delay_us(MIN_DELAY);
}

//----------------------------------------------------------------
//void CUU_writeData(uint8_t data)
//Write data to the VFD controller.
//data: byte to send
void
CUU_writeData(Noritake_VFD_CUU* this, uint8_t data) {
	this->io->write(this->io, data, true);
	_delay_us(MIN_DELAY);
}

//----------------------------------------------------------------
//uint8_t CUU_readData();
//Read command from the VFD controller.
uint8_t
CUU_readCommand(Noritake_VFD_CUU* this) {
	uint8_t data = this->io->read(this->io, false);
	_delay_us(MIN_DELAY);
	return data;
}

//----------------------------------------------------------------
//uint8_t CUU_readData();
//Read data from the VFD controller. The cursor advances in the
//same direction as if a write had occurred. The display does not
//shift even if autoscroll is enabled.
uint8_t
CUU_readData(Noritake_VFD_CUU* this) {
	uint8_t data = this->io->read(this->io, true);
	_delay_us(MIN_DELAY);
	return data;
}

//----------------------------------------------------------------
//uint8_t CUU_readAddress();
//Return the address of either DDRAM (cursor address) or CGRAM.
//Unless you have used CUU_command() to set the CGRAM address and
//you have not done an operation to set the cursor position since,
//then the DDRAM address will be returned. When a CGRAM address
//is read bit6 (0x40) will be set.
uint8_t
CUU_readAddress(Noritake_VFD_CUU* this) {
	return CUU_readCommand(this) & ~0x80;
}

//----------------------------------------------------------------
//uint8_t CUU_readRAM()
//Return the data from either DDRAM (cursor address) or CGRAM.
//Unless you have used CUU_command() to set the CGRAM address and
//you have not done an operation to set the cursor position since,
//then the data from DDRAM will be returned. The cursor advances
//in the same direction as if a write had occurred. The display
// does not shift even if autoscroll is enabled.
uint8_t
CUU_readRAM(Noritake_VFD_CUU* this) {
	return CUU_readData(this);
}

void
nextLine(Noritake_VFD_CUU* this) {
	uint8_t x = CUU_readAddress(this);
    if (this->is_cu20045_uw4j) {
    	if (x < 0x20)
    		x = 0x20;
    	else if (x<0x40)
    		x = 0x40;
    	else if (x<0x60)
    		x = 0x60;
    	else if (x<0x80)
    		x = 0x00;
    	CUU_setCursor(this, x);
    } else {
		if (x < 0x14) // line 0
			x = 0x40;
		else if (this->lines > 2 && 0x40<=x && x<0x54) // line 1
			x = 0x14;
		else if (this->lines > 3 && 0x14<=x && x<=0x28) // line 2
			x = 0x54;
		else
			x = 0x00;
		CUU_setCursor(this, x);
    }
	_delay_us(MIN_DELAY);
}

//----------------------------------------------------------------
//Print an arbitrary character (including 0x00).
//The effects of printing past the end of the line depends on the model.
//data:	character to print
void
print_char(Noritake_VFD_CUU* this, char data) {
	CUU_writeData(this, data);
}

//----------------------------------------------------------------
//Print a NULL-terminated string.
//The effects of printing past the end of the line depends on the model.
//str: a null-terminated string
void
print_str(Noritake_VFD_CUU* this, const char *str) {
	while (*str)
		print_char(this, *str++);
}

//----------------------------------------------------------------
//Print arbitrary characters (including 0x00).
//The effects of printing past the end of the line depends on the model.
//buffer:	characters to print
//size:	number of characters to print
void
print_buf(Noritake_VFD_CUU* this, const uint8_t *buffer, size_t size) {
	while (size--)
		print_char(this, (char) *buffer++);
}

// ----------------------------------------------------------------
//Print an arbitrary character (including 0x00) and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//data:	character to print
void
println_char(Noritake_VFD_CUU* this, char data) {
	print_char(this, data);
	nextLine(this);
}

// ----------------------------------------------------------------
//Print a NULL-terminated string and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//str: a null-terminated string
void
println_str(Noritake_VFD_CUU* this, const char *str) {
	print_str(this, str);
	nextLine(this);
}

// ----------------------------------------------------------------
//Print arbitrary characters (including 0x00) and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//buffer:	characters to print
//size:	number of characters to print
void
println_buf(Noritake_VFD_CUU* this, const uint8_t *buffer, size_t size) {
	print_buf(this, buffer, size);
	nextLine(this);
}

void
printNumber_ulong(Noritake_VFD_CUU* this, unsigned long number, int base_) {
	if (number/base_)
		printNumber_ulong(this, number/base_, base_);
	print_char(this, "0123456789ABCDEF"[number%base_]);
}

void
printNumber_long(Noritake_VFD_CUU* this, long number, int base_) {
	if (number/base_)
		printNumber_long(this, number/base_, base_);
	print_char(this, "0123456789ABCDEF"[number%base_]);
}

//----------------------------------------------------------------
//Print a number.
//number:	number to print
//base:	base to print in (2-16)
void
print_int(Noritake_VFD_CUU* this, int number, int base_) {
	if (number < 0) {
		print_char(this, '-');
		number = -number;
		printNumber_long(this, (long)number, base_);
	} else
		printNumber_long(this, (long)number, base_);
}

//----------------------------------------------------------------
//Print a number.
//number:	number to print
//base:	base to print in (2-16)
void
print_uint(Noritake_VFD_CUU* this, unsigned int number, int base_) {
	printNumber_ulong(this, (unsigned long)number, base_);
}

//----------------------------------------------------------------
//Print a number.
//number:	number to print
//base:	base to print in (2-16)
void
print_long(Noritake_VFD_CUU* this, long number, int base_) {
	if (number < 0) {
		print_char(this, '-');
		number = -number;
		printNumber_long(this, (long)number, base_);
	} else
		printNumber_long(this, (long)number, base_);
}

//----------------------------------------------------------------
//Print a number.
//number:	number to print
//base:	base to print in (2-16)
void
print_ulong(Noritake_VFD_CUU* this, unsigned long number, int base_) {
	printNumber_ulong(this, number, base_);
}

// ----------------------------------------------------------------
//Print a number and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//number:	number to print
//base:	base to print in (2-16)
void
println_int(Noritake_VFD_CUU* this, int number, int base_) {
	print_long(this, (long)number, base_);
	nextLine(this);
}

//----------------------------------------------------------------
//Print a number and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//number:	number to print
//base:	base to print in (2-16)
void
println_uint(Noritake_VFD_CUU* this, unsigned int number, int base_) {
	print_long(this, (unsigned long)number, base_);
	nextLine(this);
}

//----------------------------------------------------------------
//Print a number and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//number:	number to print
//base:	base to print in (2-16)
void
println_long(Noritake_VFD_CUU* this, long number, int base_) {
	print_long(this, number, base_);
	nextLine(this);
}

//----------------------------------------------------------------
//Print a number and go to the next line.
//If this was printed on the last line, this will return to the first line.
//The effects of printing past the end of the line depends on the model.
//number:	number to print
//base:	base to print in (2-16)
void
println_ulong(Noritake_VFD_CUU* this, unsigned long number, int base_) {
	print_ulong(this, number, base_);
	nextLine(this);
}

// ----------------------------------------------------------------
// Clear the screen. Fill DD RAM with blanks (0x20). Move the
// cursor home. Reset display shift to no shift. Set the entry mode to
// left-to-right.
void
CUU_clearScreen(Noritake_VFD_CUU* this) {
	CUU_command(this, 0x01);
	waitMS(5);
}

// ----------------------------------------------------------------
// Moves the cursor back to the home position (top-left). Display
// shift is reset to no shift.
void
CUU_home(Noritake_VFD_CUU* this) {
	CUU_command(this, 0x02 + this->bc_font);
	_delay_us(MIN_DELAY);
}

// ----------------------------------------------------------------
//Move the cursor to the given linear position. You may use this
//function to move the cursor to areas that are not part of visible
//lines for models that have them. If you attempt to set it to a
//position that is not within one of these ranges, the cursor
//will be set at the address of beginning of the next line. If
//the position is past the last line, then the cursor will be
//set to 0x00. The most highest bit of the position is ignored.
//e.g 0x92 is the same as 0x80+0x12 and 0x80, the highest bit, is ignored
//and the cursor will be set to 0x12.
//pos:	0 through 0x80
//	LINE	POS ON 2-LINE   POS ON 4-LINE	POS ON UW4J
//	0       0x00 - 0x27     0x00 - 0x13     0x00 - 0x13
//	1       0x40 - 0x67     0x40 - 0x53     0x20 - 0x33
//	2                       0x14 - 0x27     0x40 - 0x53
//	3                       0x54 - 0x67     0x60 - 0x73
void
CUU_setCursor(Noritake_VFD_CUU* this, uint8_t pos) {
	CUU_command(this, 0x80|pos);
	_delay_us(500);
}

// ----------------------------------------------------------------
//Move the cursor to the given position. You cannot use this function
//to move the cursor to areas that are not part of visible lines.
//col: column to move to
//line: line to move to
void
CUU_setCursor_2d(Noritake_VFD_CUU* this, uint8_t col, uint8_t line) {
	if (col < this->cols && line < this->lines) {
        if (this->is_cu20045_uw4j)
    		switch (line) {
    		case 0:
    			CUU_setCursor(this, 0x00 + col);
    			break;
    		case 1:
    			CUU_setCursor(this, 0x20 + col);
    			break;
    		case 2:
    			CUU_setCursor(this, 0x40 + col);
    			break;
    		case 3:
    			CUU_setCursor(this, 0x60 + col);
    			break;
    		}
        else
    		switch (line) {
    		case 0:
    			CUU_setCursor(this, 0x00 + col);
    			break;
    		case 1:
    			CUU_setCursor(this, 0x40 + col);
    			break;
    		case 2:
    			CUU_setCursor(this, 0x14 + col);
    			break;
    		case 3:
    			CUU_setCursor(this, 0x54 + col);
    			break;
    		}
	}
}

void
setDisplay(Noritake_VFD_CUU* this) {
	CUU_command(this, 8 + this->display * 4 + this->cursor * 2 + this->blink);
	_delay_us(MIN_DELAY);
}

// ----------------------------------------------------------------
// Turn the display on.
void
CUU_displayOn(Noritake_VFD_CUU* this) {
	this->display = true;
	setDisplay(this);
}

// ----------------------------------------------------------------
// Turn the display off. This sends the module into a low power
// consumption mode. See the manual for your module for details.
void
CUU_displayOff(Noritake_VFD_CUU* this) {
	this->display = false;
	setDisplay(this);
}

//----------------------------------------------------------------
//Turn the underline cursor on. The cursor will not be visible
//on the following models: CU20045-UW4J, CU20045-UW5J, CU20045-UW5A,
//CU20045-UW7J, CU20049-UW2J, CU20049-UW2A
void
CUU_cursorOn(Noritake_VFD_CUU* this) {
	this->cursor = true;
	setDisplay(this);
}

// ----------------------------------------------------------------
// Turn the underline cursor off.
void
CUU_cursorOff(Noritake_VFD_CUU* this) {
	this->cursor = false;
	setDisplay(this);
}

// ----------------------------------------------------------------
// Enable the full-cell block (blinking) cursor.
void
CUU_blinkOn(Noritake_VFD_CUU* this) {
	this->blink = true;
	setDisplay(this);
}

// ----------------------------------------------------------------
// Disable the full-cell block (blinking) cursor.
void
CUU_blinkOff(Noritake_VFD_CUU* this) {
	this->blink = false;
	setDisplay(this);
}

// ----------------------------------------------------------------
// Scroll the display window to the left. The previous leftmost
// character will no longer be displayed and the previous rightmost
// character will now be the second from the right.
void
CUU_scrollDisplayLeft(Noritake_VFD_CUU* this) {
	CUU_command(this, 0x18);
	_delay_us(MIN_DELAY);
}

// ----------------------------------------------------------------
// Scroll the display window to the right. The previous rightmost
// character will no longer be displayed and the previous leftmost
// character will now be the second.
void
CUU_scrollDisplayRight(Noritake_VFD_CUU* this) {
	CUU_command(this, 0x1c);
	_delay_us(MIN_DELAY);
}

void
setDirection(Noritake_VFD_CUU* this) {
	CUU_command(this, 4 + 2 * (!this->rightToLeft) + this->autoscroll);
	_delay_us(MIN_DELAY);
}

// ----------------------------------------------------------------
// Set the entry mode to move the cursor to the right after a
// character has been inserted.
void
CUU_leftToRight(Noritake_VFD_CUU* this) {
	this->rightToLeft = false;
	setDirection(this);
}

// ----------------------------------------------------------------
// Set the entry mode to move the cursor to the left after a
// character has been inserted.
void
CUU_rightToLeft(Noritake_VFD_CUU* this) {
	this->rightToLeft = true;
	setDirection(this);
}

// ----------------------------------------------------------------
// Automatically scroll the display whenever a character is printed.
void
CUU_autoscroll(Noritake_VFD_CUU* this) {
	this->autoscroll = true;
	setDirection(this);
}

// ----------------------------------------------------------------
// Do not automatically scroll the display whenever a character is
// printed.
void
CUU_noAutoscroll(Noritake_VFD_CUU* this) {
	this->autoscroll = false;
	setDirection(this);
}

// ----------------------------------------------------------------
// Create a character in CG RAM.
// num:	character number may be 0 through 7.
// data:	bitmap data in HD44780 format.
// 	Each byte represents a line.
// 	The five least significant bits are the pixel values.
// 	The most significant bit is the leftmost.
// 	Seven bytes are used for bitmap data.
// 	The fifth bit of the eighth byte sets the whole row if set.
// 	Other bits in the eighth byte are ignored.
//	The eighth row will not be visible on the following models:
//	CU20045-UW4J, CU20045-UW5J, CU20045-UW5A,
//	CU20045-UW7J, CU20049-UW2J, CU20049-UW2A
void
CUU_createChar(Noritake_VFD_CUU* this, uint8_t num, uint8_t *bits) {
	if (num >= 8)
		return;
	uint8_t	addr = CUU_readAddress(this);
	uint8_t oldDir = this->rightToLeft;
	
	if (this->rightToLeft)
		CUU_leftToRight(this);
	
	CUU_command(this, 0x40 + num * 8);	// set CGRAM address
	for (uint8_t i=0; i<8; i++)
		CUU_writeData(this, bits[i]);
	
	if (oldDir)
		CUU_rightToLeft(this);
	CUU_command(this, 0x80|addr);	// restore DDRAM address
	_delay_us(MIN_DELAY);
}

//----------------------------------------------------------------
//void CUU_readChar(uint8_t *data, uint8_t num);
//Read a character (8 bytes) from CGRAM
//num:	character number may be 0 through 7.
//data:	bitmap data in HD44780 format.
//	Each byte represents a line.
//	The five least significant bits are the pixel values.
//	The most significant bit is the leftmost.
//	Seven bytes are used for bitmap data.
//	The fifth bit of the eighth byte sets the whole row if set.
//	Other bits in the eighth byte are ignored.
void
CUU_readChar(Noritake_VFD_CUU* this, uint8_t *data, uint8_t num) {
	if (num >= 8)
		return;
		
	uint8_t	addr = CUU_readAddress(this);
	uint8_t oldDir = this->rightToLeft;
	
	if (this->rightToLeft)
		CUU_leftToRight(this);

	CUU_command(this, 0x40 + num * 8);	// set CGRAM address
	for (uint8_t i = 0; i < 8; i++)
		data[i] = CUU_readData(this);	

	if (oldDir)
		CUU_rightToLeft(this);
	CUU_command(this, 0x80|addr);	// restore DDRAM address
	_delay_us(MIN_DELAY);
}

//----------------------------------------------------------------
// uint8_t CUU_readBusy();
//Return the busy flag. 1 indicates the device is busy.
uint8_t
CUU_readBusy(Noritake_VFD_CUU* this) {
	return (CUU_readCommand(this) & 0x80) != 0;
}

// ----------------------------------------------------------------
// Set the brightness of the VFD module.
// brightness: brightness value in percent: 25, 50, 75, or 100.
// 	Models with brightness boost (CU-UX models) can accept
//	values: 50, 100, 150, or 200.
//	The following models support brightness boost:
//		CU16025-UX6J
//		CU16025-UX6A
//		CU20025-UX1J
void
CUU_brightness(Noritake_VFD_CUU* this, int brightness) {
    if (this->bc_vfd) {
        if (brightness <= 0 || brightness > 100) return;
        CUU_command(this, 0x30 + (10000 - brightness*100) / 625);
    	_delay_us(MIN_DELAY);
    } else {
        const register int m = this->hasBrightnessBoost? 2 : 1;
        if (brightness <= 0 || brightness > 100 * m) return;
        // brightness byte is sent after Function Set
        CUU_command(this, /*this->io->is8bit() ? 0x30 : */ 0x20);
    	_delay_us(MIN_DELAY);
    	CUU_writeData(this, (100 * m - brightness) / (25 * m));
    	_delay_us(MIN_DELAY);
    }
}
