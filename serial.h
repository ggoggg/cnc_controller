/* 
 * File:   serial.h
 * Author: user
 *
 * Created on January 6, 2019, 1:45 PM
 */

#ifndef SERIAL_H
#define	SERIAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#include <xc.h>

#define _XTAL_FREQ 20000000

void UART_Read_Text(char *Output, unsigned int length);
char UART_Read();
char UART_Data_Ready();
void UART_Write_Text(char *text);
char UART_TX_Empty();
void UART_Write(char data);
int UART_Init(const long int baudrate);

#endif	/* SERIAL_H */
