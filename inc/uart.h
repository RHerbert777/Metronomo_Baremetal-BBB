#ifndef UART_H
#define UART_H
#include <stdint.h>

/*******************************************************
 * _  _   ___  
 * | || | | _ \    Arquivo: uart.h
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: 
 *
 *******************************************************/

// prcm
#define CM_WKUP_UART0_CLKCTRL  (0x44E00400 + 0x00) // Endereço de energia da UART0

// Registradores do Control Module para Multiplexação (Pinos físicos da placa)
#define CONTROL_MODULE_BASE    (0x44E10000)
#define CONF_UART0_RXD         (0x970)
#define CONF_UART0_TXD         (0x974)

// Registradores internos da UART0
#define UART_RHR    (0x00)
#define UART_THR    (0x00)
#define UART_DLL    (0x00)
#define UART_DLH    (0x04)
#define UART_LCR    (0x0C)
#define UART_MDR1   (0x20)
#define UART_LSR    (0x14)

//Setup
void uartSetup(void);

//Saida
void uartPutChar(char c);
void uartPutString(char *str);

//entrada
uint8_t uartAvailable(void); //flag que verifica teclado
char uartGetChar(void);

void uartPutNumber(uint32_t num);
#endif