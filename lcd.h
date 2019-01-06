/* 
 * File:   lcd.h
 * Author: user
 *
 * Created on January 5, 2019, 1:09 AM
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#define _XTAL_FREQ 20000000

#define RS PORTBbits.RB1  /*PIN 0 of PORTB is assigned for register select Pin of LCD*/
#define EN PORTBbits.RB2  /*PIN 1 of PORTB is assigned for enable Pin of LCD */
#define ldata PORTB  /*PORTB(PB4-PB7) is assigned for LCD Data Output*/ 
#define LCD_Port TRISB  /*define macros for PORTB Direction Register*/

void LCD_Init(void);                   /*Initialize LCD*/
void LCD_Command(unsigned char );  /*Send command to LCD*/
void LCD_Char(unsigned char x);    /*Send data to LCD*/
void LCD_String(const char *);     /*Display data string on LCD*/
void LCD_String_xy(char, char , const char *);
void LCD_Clear(void); 

#endif	/* LCD_H */

