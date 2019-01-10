/* 
 * File:   cnc.c
 * Author: user
 *
 * Created on January 4, 2019, 12:05 AM
 */
#define _XTAL_FREQ 20000000

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lcd.h"
#include "serial.h"

// BEGIN CONFIG
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
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

typedef struct{
    uint8_t cmd_type;
    uint8_t cmd_num;
    float    x_param;
    float    y_param;
    float    z_param;
    uint8_t f_param;
    float r_param;
}command_t;

command_t command;

void StepX(long point, uint8_t speed, uint8_t dir);
void StepY(long point, uint8_t speed, uint8_t dir);
void StepZ(long point, uint8_t speed, uint8_t dir);

coordinates_t current_position = {0,0,0,0,0,0,0,0,0};

const char *sep_tok = " ";

void delay_us(unsigned int val){
    while(val > 0){
        __delay_us(1);
        val--;
    }
}

void putch(char data){
    while(!TXIF) continue;
    TXREG = data;
}
/*
double _atol(char *data){
    double ret=0;
    long part1=0;
    long part2=0;
    bool neg = false;
    bool dec = false;
    uint8_t dec_cnt=0;
    uint8_t i=0;
    while(*data!=NULL){
        switch(data[0]){
            case '.' : if(dec) return NULL;
                        dec = true;
                        break;
            case '-' : if(neg) return NULL;
                            neg = true;
                            break;
            case '0' : if(!dec) part1 = (part1*10)+0; else { part2 = (part2*10)+0; dec_cnt++;} break;
            case '1' : if(!dec) part1 = (part1*10)+1; else { part2 = (part2*10)+1; dec_cnt++;} break;
            case '2' : if(!dec) part1 = (part1*10)+2; else { part2 = (part2*10)+2; dec_cnt++;} break;
            case '3' : if(!dec) part1 = (part1*10)+3; else { part2 = (part2*10)+3; dec_cnt++;} break;
            case '4' : if(!dec) part1 = (part1*10)+4; else { part2 = (part2*10)+4; dec_cnt++;} break;
            case '5' : if(!dec) part1 = (part1*10)+5; else { part2 = (part2*10)+5; dec_cnt++;} break;
            case '6' : if(!dec) part1 = (part1*10)+6; else { part2 = (part2*10)+6; dec_cnt++;} break;
            case '7' : if(!dec) part1 = (part1*10)+7; else { part2 = (part2*10)+7; dec_cnt++;} break;
            case '8' : if(!dec) part1 = (part1*10)+8; else { part2 = (part2*10)+8; dec_cnt++;} break;
            case '9' : if(!dec) part1 = (part1*10)+9; else { part2 = (part2*10)+9; dec_cnt++;} break;
            default : return NULL;
        }
        *data++;
    }
    long div = 0;
    for(i=0;i<dec_cnt;i++) div *= 10;
    ret = (double)part1 + ((double)part2/div);
    if(neg) return ret*-1;
    else return ret;
}
*/
void G01(char *x_str,char *y_str,char *speed_str){
    //long x=_atol(x_str);
    //long y=_atol(y_str);
    //uint8_t speed = (int)_atol(speed_str);
    //printf("\nGoing to %d,%d,%d\r\n",x, y, speed);
//    if(b.x != current_position.x && b.y == current_position.y)
//        StepX(b.x,speed,1);
//    else if (b.x == current_position.x && b.y != current_position.y)
//        StepY(b.y,speed,1);
//    else {
    
//    }
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

void cncCommand(){
    
    printf("\npoints %.4f\n",command.x_param);
    //if(command.cmd_type == 'G'){
    //    switch(command.cmd_num){
    //        case 1 : break;
    //        case 2 : break;
    //        default: break;
    //    }
    //}
}

void parseCMD(char *buffer){
    int p_cnt=0;
    char *ptr;
    memset(&command,0,sizeof(command));
    if(*buffer == '?'){
        printf("Current Position: X-%d Y-%d\r\n>",current_position.x,current_position.y);
    }
    else if (*buffer == 'G'){
        ptr = strtok(buffer,sep_tok);
        command.cmd_type = ptr[0];
        command.cmd_num = atoi(ptr+1);
        p_cnt++;
        while(ptr !=NULL){
            ptr = strtok(NULL,sep_tok);
            switch(ptr[0]){
                case 'X' : command.x_param = atof(ptr+1); break; 
                case 'Y' : command.y_param = atof(ptr+1); break;
                case 'Z' : command.z_param = atof(ptr+1); break;
                case 'F' : command.f_param = atoi(ptr+1); break;
                case 'R' : command.r_param = atof(ptr+1); break;
                default : break;
            }
        }
        cncCommand();
    }
}

void main(void) {
    
//    OSCCONbits.OSTS = 1;
//    OSCCONbits.SCS = 0;
    
    char str[16] = {0};
    char rxbuff[32] = {0};
//    unsigned char rxbyte[1] = {0};
    __delay_ms(1000);
    
    ANSEL = 0;
    PORTA = 0;
    TRISA = 0xFF;
    GIE = 0;
    
    
    ANSELH = 0;
    IOCB = 0;
    PORTD = 0;
    TRISD = 0;
    
    PORTB = 0;
    TRISB = 1;
    
    
    PORTC = 0;
    TRISC = 0xff;    
    
    LCD_Init();  /*Initialize LCD to 5*8 matrix in 4-bit mode*/
    LCD_Clear();
    
    LCD_String_xy(1,0,"HELLO"); /*Display string on 2nd row,1st location*/ 
    
    UART_Init(9600);    
    printf("System is Ready!\r\n>");
    
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
    //UART_Read();
      int cnt=0; 
      int i;
    while(1){
        while(!RCIF) NOP();
        rxbuff[cnt] = RCREG;
        UART_Write(rxbuff[cnt]);
        if(rxbuff[cnt]==0x0d || rxbuff[cnt]==0x0a || cnt>32) {
            rxbuff[cnt] = 0;
            __delay_ms(10);
            LCD_String_xy(1,1,rxbuff);
            cnt=0;
            parseCMD(rxbuff);
            memset(rxbuff,0,sizeof(rxbuff));
        }
        else {
             cnt++;
        }
    }
    
    
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