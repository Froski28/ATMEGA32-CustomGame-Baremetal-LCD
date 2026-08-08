/*
 * my_lcd.h
 *
 * Created: 4/27/2025 2:12:02 PM
 *  Author: jujuf
 */ 


#ifndef MY_LCD_H_
#define MY_LCD_H_

#include "avr.h"

void wait_microsecond(volatile int am);

void pulse(int ms, int fast);

void LCD_write_data_nocheckbusy(uint8_t data, uint8_t rs, int ms, int fast);

uint8_t LCD_read_data(uint8_t rs);

uint8_t check_busy();

void LCD_write_data(uint8_t data, uint8_t rs, int ms, int fast);

void fourBit_LCDInit();

void LCD_TurnOnOff(uint8_t on, uint8_t cursor, uint8_t blink);

void LCD_Lines_Font(uint8_t double_lined, uint8_t font_five_by_ten);

void LCD_place_cursor(uint8_t pos, uint8_t isSecondRow);

uint8_t LCD_get_at_pos(uint8_t pos, uint8_t isSecondRow);

uint8_t LCD_check_end_pos(uint8_t isSecondRow);

void LCD_write_char(uint8_t data, uint8_t pos, uint8_t isSecondRow);

void LCD_shift_display(uint8_t isRight, uint8_t isSecondRow, uint8_t isRotating);

void LCD_shift_display_rows(uint8_t isRight, uint8_t isRotating);

void LCD_cmd_shift(uint8_t right_else_left);

void LCD_cursor_shift(uint8_t right_else_left);

void LCD_clear();

void LCD_write_string(const char* str, uint8_t starting_pos, uint8_t isSecondRow);

void LCD_clear_line(uint8_t isSecondRow);

uint8_t LCD_get_cursor_position(uint8_t isSecondRow);




#endif /* MY_LCD_H_ */