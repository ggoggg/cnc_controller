/* 
 * File:   cnc.c
 * Author: user
 *
 * Created on January 4, 2019, 12:05 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"
#define _XTAL_FREQ 20000000



// BEGIN CONFIG
#pragma config FOSC = XT // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG

#define CNC_EN      PORTDbits.RD0
#define CNC_STEPX   PORTDbits.RD1
#define CNC_DIRX    PORTDbits.RD2
#define CNC_STEPY   PORTDbits.RD3
#define CNC_DIRY    PORTDbits.RD4
#define CNC_STEPZ   PORTDbits.RD5
#define CNC_DIRZ    PORTDbits.RD6

#define CNC_MAX_LIMITX  PORTAbits.RA0
#define CNC_MIN_LIMITX  PORTAbits.RA1
#define CNC_MAX_LIMITY  PORTAbits.RA2
#define CNC_MIN_LIMITY  PORTAbits.RA3
#define CNC_MAX_LIMITZ  PORTAbits.RA4
#define CNC_MIN_LIMITZ  PORTAbits.RA5

/*
 * 
 */
/*
void Step(uint8_t motor,unsigned long steps,uint8_t dir);

int main(int argc, char** argv) {
    PORTD = 0;
    TRISD = 0;
    
    while(1){
        if (CNC_EN)  CNC_EN=0;
        else CNC_EN = 1;
        __delay_ms(100);
        
        Step(1,20000,1);
        //Step(2,300,0);
        //Step(3,500,0);
        Step(1,20000,0);
        CNC_EN = 1;
    }
    
    
    return (EXIT_SUCCESS);
}

void Step(uint8_t motor,unsigned long steps,uint8_t dir){
    
    CNC_EN = 0;
    unsigned long i=0;
    if(motor==1){
        CNC_DIRX = dir;
        for(i=0;i<steps;i++){
            __delay_us(40);
            CNC_STEPX = 1;
            __delay_us(20);
            CNC_STEPX = 0;
        }
    }
    else if(motor==2){
        CNC_DIRY = dir;
        for(i=0;i<steps;i++){
            __delay_ms(10);
            CNC_STEPY = 1;
            __delay_ms(5);
            CNC_STEPY = 0;
        }    
    }
    else if(motor==3){
        CNC_DIRX = dir;
        for(i=0;i<steps;i++){
            __delay_us(10);
            CNC_STEPZ = 1;
            __delay_us(5);
            CNC_STEPZ = 0;
        }    
    }
}
*/

typedef struct{
    signed long x;
    signed long y;
    signed long z;
    bool xlimit_max;
    bool xlimit_min;
    bool ylimit_max;
    bool ylimit_min;
    bool zlimit_max;
    bool zlimit_min;

}coordinates_t;

void StepX(long point, uint8_t speed, uint8_t dir);
void StepY(long point, uint8_t speed, uint8_t dir);
void StepZ(long point, uint8_t speed, uint8_t dir);

coordinates_t current_position = {0,0,0,0,0,0,0,0,0};

void delay_us(unsigned int val){
    while(val > 0){
        __delay_us(1);
        val--;
    }
}

void G01(coordinates_t b,uint8_t speed){
    if(b.x != current_position.x && b.y == current_position.y)
        StepX(b.x,speed,1);
    else if (b.x == current_position.x && b.y != current_position.y)
        StepY(b.y,speed,1);
    else {
    
    }
}

void GotoX(signed long point,uint8_t speed){
    long i=0;
    unsigned int wait = (10*100)/speed;
    char str[16] = {0};

    if(point>current_position.x) {
        CNC_DIRX = 0;
        LCD_String_xy(1,0,"point>pos");
    }
    else {
        CNC_DIRX = 1;
        LCD_String_xy(1,0,"point<pos");
    }
    while(current_position.x!=point) {
        if (!CNC_MIN_LIMITX) current_position.xlimit_min = true;
        if (!CNC_MAX_LIMITX) current_position.xlimit_max = true;
        if     (current_position.xlimit_min && CNC_DIRX == 1) break;
        else if(current_position.xlimit_max && CNC_DIRX == 0) break;
        else {
            delay_us(wait);
            CNC_STEPX = 1;
            __delay_us(5);
            CNC_STEPX = 0;
           if(CNC_DIRX==0) {
               current_position.x++;
           }
           else {
               current_position.x--;        
           }
        }
    }
}

void StepX(long steps, uint8_t speed, uint8_t dir){
    
    CNC_DIRX = dir;
    long i=0;
    unsigned int wait = (10*100)/speed;
    for(i=0;i<steps;i++){
        //if(CNC_LIMITX==0) break;
    //while(current_position.x!=point){
       delay_us(wait);
       CNC_STEPX = 1;
       __delay_us(5);
       CNC_STEPX = 0;
       if(!CNC_DIRX) current_position.x++;
       else current_position.x--;
    }
}

void StepY(long point, uint8_t speed, uint8_t dir){
    
    CNC_DIRY = dir;
    
    unsigned int wait = (20*100)/speed;
    
    while(current_position.y!=point){
       delay_us(wait);
       CNC_STEPY = 1;
       __delay_us(5);
       CNC_STEPY = 0;
       if(CNC_DIRY) current_position.y++;
       else current_position.y--;
    }
}

void StepZ(long point, uint8_t speed, uint8_t dir){
    
    CNC_DIRZ = dir;
    unsigned int wait = (20*100)/speed;
    
    while(current_position.z!=point){
       delay_us(wait);
       CNC_STEPZ = 1;
       __delay_us(5);
       CNC_STEPZ = 0;
       if(CNC_DIRZ) current_position.z++;
       else current_position.z--;
    }    
}

void main(void) {
    char str[16] = {0};
    __delay_ms(500);
    
    ANSEL = 0;
    PORTA = 0;
    TRISA = 0xFF;
    
    ANSELH = 0;
    IOCB = 0;
    PORTD = 0;
    TRISD = 0;
    
    PORTB = 0;
    TRISB = 1;
    
    LCD_Init();  /*Initialize LCD to 5*8 matrix in 4-bit mode*/
    LCD_Clear();
    
    LCD_String_xy(1,0,"HELLO"); /*Display string on 2nd row,1st location*/ 
    LCD_String_xy(2,0,"POS X: ");
    LCD_String_xy(3,0,"POS Y: ");
    LCD_String_xy(4,0,"POS Z: ");

    sprintf(str,"%8lu",current_position.x);
    LCD_String_xy(2,7,str);
    str[0] = 0;

    if (!CNC_MIN_LIMITX) current_position.xlimit_min = true;
    if (!CNC_MAX_LIMITX) current_position.xlimit_max = true;

    coordinates_t to;
    to.x = 10;
    to.y = 10;
    to.z = 10;
    
    //G01(to,100);
    while(1){
        if(!PORTBbits.RB0){
            //StepX(10000,100,0);
            GotoX(10000,100);
        }
    //StepX(10000,100,0);
    
    //sprintf(str,"%8lu",current_position.x);
    //LCD_String_xy(2,7,str);
        if(current_position.xlimit_max || current_position.xlimit_min) LCD_String_xy(2,15,"*");
        else LCD_String_xy(2,15," ");
    str[0] = 0;
    }
    while(1){
        __delay_ms(1000);
    }
    //StepY(10,100);
    //StepZ(10,100);
    return;
}