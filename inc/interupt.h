#ifndef INTERUPT_H
#define INTERUPT_H

#include "soc_AM335x.h"
#include "hw_types.h"
#include <stdint.h>
extern volatile uint8_t flag_start;
extern volatile uint8_t flag_SET;
extern volatile uint8_t flag_UP;
extern volatile uint8_t flag_DOWN;

extern volatile uint32_t sistema_ms;

// Offsets para Inicialização (Sistema)
#define INTC_SYSCONFIG      (0x10)
#define INTC_SYSSTATUS      (0x14)
#define INTC_THRESHOLD      (0x68)

// Máscaras de Bits
#define INTC_SYSCONFIG_SOFTRESET   (1 << 1) // Bit 1 força o reset
#define INTC_SYSSTATUS_RESETDONE   (1 << 0) // Bit 0 indica reset concluído
#define INTC_THRESHOLD_PRIORITYTHRESHOLD  0xFF // Permite todas as prioridades

//tipos de detecção de bordas
#define EDGE_RISING   (0x1) //subida
#define EDGE_FALLING  (0x2) //descida
#define EDGE_BOTH     (0x3) //descida e subida

//INTC register
#define INTC_SIR_IRQ        (0x40)
#define INTC_CONTROL        (0x48)
#define INTC_MIR_CLEAR0     (0x88)
#define INTC_MIR_CLEAR1     (0xA8) 
#define INTC_MIR_CLEAR2     (0xC8) 
#define INTC_MIR_CLEAR3     (0xE8)  

//definiçoes para interupçoes de gpio
#define GPIO_IRQSTATUS_0       0x2C   // Lê quem disparou e limpa a flag de interrupção
#define GPIO_IRQSTATUS_SET_0   0x34   // Habilita a geração de interrupção no pino
#define GPIO_IRQSTATUS_CLR_0   0x3C   // Desabilita a geração de interrupção no pino
#define GPIO_FALLINGDETECT     0x148  // Configura gatilho para Borda de Descida (Pull-Up)
#define GPIO_RISINGDETECT      0x14C  // Configura gatilho para Borda de Subida (Pull-Down)

void gpioInterruptSetup(uint32_t modulo_gpio, uint8_t pino, uint8_t tipo_gatilho);
void ISR_Handler();
void delay_ms_interrupt(unsigned int mSec);
void IntAINTCInit(void);
void AINTC_Destrancar_Interrupcao(uint32_t modulo_base);
void Timer7_Start_System_Tick(void) ;

#endif