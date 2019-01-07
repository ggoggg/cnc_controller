#include "lcd.h"
#include <xc.h>

void LCD_Init()
{
    LCD_Port = 1;       /*PORT as Output Port*/
    __delay_ms(500);        /*15ms,16x2 LCD Power on delay*/
    LCD_Command(0x02);  /*send for initialization of LCD 
                          for nibble (4-bit) mode */
    LCD_Command(0x28);  /*use 2 line and 
                          initialize 5*8 matrix in (4-bit mode)*/
	LCD_Command(0x01);  /*clear display screen*/
    LCD_Command(0x0c);  /*display on cursor off*/
    //LCD_Command(0x0f);
	LCD_Command(0x06);  /*increment cursor (shift cursor to right)*/	   
}

void LCD_Command(unsigned char cmd )
{
	ldata = (ldata & 0x0f) |(0xF0 & cmd);  /*Send higher nibble of command first to PORT*/ 
	RS = 0;  /*Command Register is selected i.e.RS=0*/ 
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/ 
	NOP();
	EN = 0;
	__delay_ms(1);
    ldata = (ldata & 0x0f) | (cmd<<4);  /*Send lower nibble of command to PORT */
	EN = 1;
	NOP();
	EN = 0;
	__delay_ms(3);
}

void LCD_Char(unsigned char dat)
{
	ldata = (ldata & 0x0f) | (0xF0 & dat);  /*Send higher nibble of data first to PORT*/
	RS = 1;  /*Data Register is selected*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	__delay_ms(1);
    ldata = (ldata & 0x0f) | (dat<<4);  /*Send lower nibble of data to PORT*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	__delay_ms(3);
}

void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    }
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row==1)
    {
        location=(0x80) | ((pos) & 0x0f);  /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else if(row==2){
        location=(0xc0) | ((pos) & 0x0f);  /*Print message on 1st row and desired location*/
        LCD_Command(location);    
    }
    else if(row==3){
        location=(0x90) | ((pos) & 0x0f);  /*Print message on 1st row and desired location*/
        LCD_Command(location);    
    }
    else if(row==4){
        location=(0xD0) | ((pos) & 0x0f);  /*Print message on 1st row and desired location*/
        LCD_Command(location);    
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f);  /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }
    LCD_String(msg);
}
void LCD_Clear()
{
   	LCD_Command(0x01);  /*clear display screen*/
    __delay_ms(3);
}
