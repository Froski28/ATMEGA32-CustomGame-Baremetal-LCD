/*
 * my_lcd.c
 *
 * Created: 4/27/2025 2:04:17 PM
 *  Author: jujuf
 */

#include "my_lcd.h"

void wait_microsecond(volatile int am)
{
	for (volatile int i=0; i < am; ++i) NOP();
}

void pulse(int ms, int fast)
{
	// Set Enable to true
	SET_BIT(PORTB, PB2);
	// Delay Enable
		//(ms) ? avr_wait(ms): wait_microsecond();
	(fast) ? wait_microsecond(1): avr_wait(2);											///////////// FIX TIMING ISSUES HERE
	//avr_wait(1);
	// Set Enable back to false
	CLR_BIT(PORTB, PB2);
}

/** RS - PB0, RW - PB1, E - PB2, DB5-7 - PB3-PB6 - (00011110 -> 01111000) - 0x78 
	Note - Data is in LOWER 4 BITS											*/
static void _send_4_bit(uint8_t data, int ms, int fast)
{
	// Set DDRD to output mode for PB3-PB6
	DDRB |= 0x78;
	// Set Port B to data
	PORTB &= ~(0x78); // CLEARS the bits previous data
	PORTB |= (data << 3); // Since data pins are PB3-PB6
	pulse(ms, fast);
}

/** RS - PB0, RW - PB1, E - PB2, DB5-7 - PB3-PB6 */
void LCD_write_data_nocheckbusy(uint8_t data, uint8_t rs, int ms, int fast)
{
	// Set RS, RW, E to output mode
	DDRB |= 0x07;
	// Check if RS is cmd or data. (0 cmd/instruction, 1 data/screen)
	(rs) ? SET_BIT(PORTB, PB0): CLR_BIT(PORTB, PB0);
	CLR_BIT(PORTB, PB1);
	// SET the data to send (two pulse - 4-bit)
	uint8_t upper = ((data & 0xF0) >> 4);
	uint8_t lower = (data & 0x0F);
	// Send the data
	_send_4_bit(upper, ms, fast);
		//avr_wait(500);
	_send_4_bit(lower, ms, fast);
		//avr_wait(500);
}

static uint8_t _pulse_read()
{
	// Set ENABLE TO ON
	SET_BIT(PORTB, PB2);
	// Wait 1 microsecond
	wait_microsecond(8);
	// READ DATA - MASK is 0x78 for data pins 01111000
	uint8_t data = ((PINB & 0x78) >> 3);
	// SET ENABLE TO OFF
	CLR_BIT(PORTB, PB2);
	return data;
}

uint8_t LCD_read_data(uint8_t rs)
{
	// Set AVR Pins to read mode & RS, RW, E to output mode - 00000111
	DDRB = 0x07;
	PORTB &= ~(0x78);
	// Check if RS is cmd or data. (0 cmd/instruction, 1 data/screen)
	(rs) ? SET_BIT(PORTB, PB0): CLR_BIT(PORTB, PB0);
	// Set RW to 1 to READ
	SET_BIT(PORTB, PB1);
	// Read Upper Portion
	uint8_t ans = (_pulse_read() << 4);
	// Read Lower Portion
	ans |= _pulse_read();
	return ans;
}

uint8_t check_busy()
{
	// Reading command is RS-0, RW-1
	// DB7 is FLAG
	return (LCD_read_data(0) & 0x80);
}

void LCD_write_data(uint8_t data, uint8_t rs, int ms, int fast)
{
	while (check_busy());
	LCD_write_data_nocheckbusy(data, rs, ms, fast);
}

static void _InitWrite(uint8_t data)
{
	// SET Ports to output mode
	DDRB |= 0x78;
	// Send Data
	PORTB &= ~(0x78);
	PORTB |= ((data >> 1) & 0x78);
	pulse(0, 0);
}

/** RS - PB0, RW - PB1, E - PB2, DB5-7 - PB3-PB6 */
void fourBit_LCDInit()
{
	// Set RS, RW, E to output
	DDRB = 0x07;
	// Wait 16ms
	avr_wait(16);
	// Running RS-RW-DB7-DB6-DB5-DB4 = 0-0-0-0-1-1 - 
	CLR_BIT(PORTB, PB0);
	CLR_BIT(PORTB, PB1);
	_InitWrite(0x30); // Writing 0011 0000
	// Wait 5 ms
	avr_wait(6);
	_InitWrite(0x30);
	// Wait 1ms
	avr_wait(1);
	_InitWrite(0x30);
	// SET TO 4 BIT MODE
	_InitWrite(0x20);
		//PORTB &= ~(0x78);
		//avr_wait(1000);										// HEEREEEEEEEE
	// SEND 0010 NFxx - N=1 for 2 Lines, F=1 for 5x10 else 5x8
	LCD_write_data(0x2c, 0, 3, 0); // 00010 1100													//REMEEBR TO FIX THIS FOR BUSY WHEN WORK
		//LCD_write_data_nocheckbusy(0x2c, 0, 3);
		//LCD_write_data(0x24, 0, 3); // 00010 1100
	// SEND 00001000 - Display OFF
	LCD_write_data(0x08, 0, 3, 0); // 0000 1000
		//LCD_write_data_nocheckbusy(0x08, 0, 3);
	// SEND 0x01 - Display CLEAR
	LCD_write_data(0x01, 0, 3, 0); // 0000 0001
		//LCD_write_data_nocheckbusy(0x01, 0, 3);
	// Entry Mode CMD
	LCD_write_data(0x06, 0, 0, 1); // 0000 0110
		//LCD_write_data_nocheckbusy(0x06, 0, 0);
}

void LCD_TurnOnOff(uint8_t on, uint8_t cursor, uint8_t blink)
{
	uint8_t ans = 0x08;
	if (on) SET_BIT(ans, 2);
	if (cursor) SET_BIT(ans, 1);
	if (blink) SET_BIT(ans, 0);
	LCD_write_data(ans, 0, 0, 0);
}

void LCD_Lines_Font(uint8_t double_lined, uint8_t font_five_by_ten)
{
	// 0010 NF00
	uint8_t mask = 0x20;
	if (double_lined) SET_BIT(mask, 3);
	if (font_five_by_ten) SET_BIT(mask, 2);
	LCD_write_data(mask, 0, 0, 0);
}

/** Of format setting the cursor to a specific position from 1-16. 16-32 is second row. Top left -> right */
void LCD_place_cursor(uint8_t pos, uint8_t isSecondRow)
{
	if (pos < 1 || pos > 16) return;
	uint8_t DDRAMPos = (isSecondRow) ? 0xC0: 0x80;
	DDRAMPos |= (--pos);
	LCD_write_data(DDRAMPos, 0, 0, 1);
}

uint8_t LCD_get_at_pos(uint8_t pos, uint8_t isSecondRow)
{
	if (pos) LCD_place_cursor(pos, isSecondRow);
	return LCD_read_data(1);
}

uint8_t LCD_check_end_pos(uint8_t isSecondRow)
{
	return LCD_get_at_pos(16, isSecondRow);
}

void LCD_write_char(uint8_t data, uint8_t pos, uint8_t isSecondRow)
{
	if (pos) LCD_place_cursor(pos, isSecondRow);
	LCD_write_data(data, 1, 0, 1);
}

void LCD_shift_display(uint8_t isRight, uint8_t isSecondRow, uint8_t isRotating)
{
	uint8_t beginningPos = (isRight) ? 1: 16;
	uint8_t endPos = (isRight) ? 16: 1;
	uint8_t endChar = ' ';
	if (isRotating)
		endChar = LCD_get_at_pos(endPos, isSecondRow);
	uint8_t prevChar = LCD_get_at_pos(1, isSecondRow);
	LCD_write_char(' ', beginningPos, isSecondRow);
	for (int i=(isRight) ? 2: 15; i < 17 && i > 0; i = (isRight) ? i+1: i-1){
		uint8_t currChar = LCD_get_at_pos(i, isSecondRow);
		LCD_write_char(prevChar, i, isSecondRow);
		prevChar = currChar;
		//avr_wait(1000);
	}
	LCD_write_char(endChar, beginningPos, isSecondRow);
}

void LCD_shift_display_rows(uint8_t isRight, uint8_t isRotating)
{
	uint8_t beginningPos = (isRight) ? 1: 16;
	uint8_t endPos = (isRight) ? 16: 1;
	uint8_t endChar1 = ' ', endChar2 = ' ';
	if (isRotating){
		endChar1 = LCD_get_at_pos(endPos, 0);
		endChar2 = LCD_get_at_pos(endPos, 1);
	}
	uint8_t prevChar1 = LCD_get_at_pos(beginningPos, 0);
	uint8_t prevChar2 = LCD_get_at_pos(beginningPos, 1);
	LCD_write_char(' ', beginningPos, 0);
	LCD_write_char(' ', beginningPos, 1);
	for (int i=(isRight) ? 2: 15; i < 17 && i > 0; i = (isRight) ? i+1: i-1){
		uint8_t currChar1 = LCD_get_at_pos(i, 0);
		uint8_t currChar2 = LCD_get_at_pos(i, 1);
		LCD_write_char(prevChar1, i, 0);
		LCD_write_char(prevChar2, i, 1);
		prevChar1 = currChar1;
		prevChar2 = currChar2;
		//avr_wait(1000);
	}
	LCD_write_char(endChar1, beginningPos, 0);
	LCD_write_char(endChar2, beginningPos, 1);
}

void LCD_cmd_shift(uint8_t right_else_left)
{
	uint8_t mask = 0x18;
	if (right_else_left) SET_BIT(mask, 2);
	LCD_write_data(mask, 0, 0, 1);
	CLR_BIT(mask, 3);
	if (right_else_left) CLR_BIT(mask, 2);
	LCD_write_data(mask, 0, 0, 1);
}

// 0111 1111
uint8_t LCD_get_cursor_position(uint8_t isSecondRow)
{
	uint8_t data = LCD_read_data(0);
	if (isSecondRow) data &= ~0xC0;
	else data &= ~0x80;											////////////////// TO CHECK
	return data+1;
}

void LCD_cursor_shift(uint8_t right_else_left)
{
	// 0001 0R00
	uint8_t mask = 0x10;
	if (right_else_left) SET_BIT(mask, 2);
	LCD_write_data(mask, 0, 0, 1);
}

void LCD_clear()
{
	LCD_write_data(0x01, 0, 1, 0);
}

void LCD_rotate(uint8_t isSecondRow)
{
	return;	
}

/*Must be between 0-16 chars, SIZE assumed 16 chars, 17 length for newline*/
void LCD_write_string(const char* str, uint8_t starting_pos, uint8_t isSecondRow)
{
	for (uint8_t i=0; (i < 17) && (*(str + i) != '\0'); ++i){
		uint8_t pos = (starting_pos) ? (starting_pos + i): 0;
		LCD_write_char(*(str + i), pos, isSecondRow);
	}
}

void LCD_clear_line(uint8_t isSecondRow)
{
	for (uint8_t i=1; i < 17; ++i) LCD_write_char(' ', i, isSecondRow);
}
