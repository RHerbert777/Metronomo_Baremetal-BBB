#include "soc_AM335x.h"
#include "uart.h"
#include "hw_types.h"

/*******************************************************
 * _  _   ___  
 * | || | | _ \    Arquivo: uart.c
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: 
 *
 *******************************************************/

void uartSetup(void){
    //Ativar PRCM
    HWREG(CM_WKUP_UART0_CLKCTRL);
    while ((HWREG(CM_WKUP_UART0_CLKCTRL) & 0x03) != 0x02);
    
    //multiplexação para os pinos rx e tx serial
    HWREG(CONTROL_MODULE_BASE + CONF_UART0_RXD) = 0x20; //tx para receber
    HWREG(CONTROL_MODULE_BASE + CONF_UART0_TXD) = 0x00; //rx para transmitir
    
    //baund rate
    HWREG(SOC_UART_0_REGS + UART_MDR1) = 0x07; //desativando temporariamente

    HWREG(SOC_UART_0_REGS + UART_LCR) |= (1 << 7);

    //Divisor = 48000000 / (16 * 115200) = 26
    HWREG(SOC_UART_0_REGS + UART_DLL) = 0x1A; //parte alta 26
    HWREG(SOC_UART_0_REGS + UART_DLH) = 0x00; //parte baixa

    HWREG(SOC_UART_0_REGS + UART_LCR) = 0x03;//fecha a configuração

    HWREG(SOC_UART_0_REGS + UART_MDR1) = 0x00; //religamos a uart
}

//apenas um caractere por vez 8bites
void uartPutChar(char c){
    while ((HWREG(SOC_UART_0_REGS + UART_LSR) & (1 << 5)) == 0);
    
    HWREG(SOC_UART_0_REGS + UART_THR) = c; //recebe o caractere
}

//recebe um ponteira e fica em while chamando uartPutChar
void uartPutString(char *str){
    while (*str != '\0') {
        uartPutChar(*str); 
        str++;             
    }
}

//ler o registrador de status UART_LSR se 0 não existe strng se 1 existe string
uint8_t uartAvailable(void){
    if (HWREG(SOC_UART_0_REGS + UART_LSR) & (1 << 0)) {
        return 1; // Tem caractere aguardando
    } else {
        return 0; // Caixa vazia
    }
}
//se uartAvailable verdadeiro vai no registrador de recepção implementar na main
//retorna o primeiro BYTE, ele desaparece automaticamente
char uartGetChar(void){
    return (char)HWREG(SOC_UART_0_REGS + UART_RHR);
}

void uartPutNumber(uint32_t num) {
    if (num == 0) {
        uartPutString("0");
        return;
    }
    char buffer[12]; // Suporta até 4 bilhões (32 bits)
    int i = 10;
    buffer[11] = '\0';
    
    while (num > 0 && i >= 0) {
        buffer[i] = (num % 10) + '0'; // Pega o último dígito e vira texto
        num /= 10;
        i--;
    }
    uartPutString(&buffer[i + 1]);
}