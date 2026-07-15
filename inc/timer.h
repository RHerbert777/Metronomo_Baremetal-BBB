#ifndef TIMER_H
#define TIMER_H

/*******************************************************
 * _  _   ___  
 * | || | | _ \    Arquivo: timer.h
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: Cabeçalho do modulo de timer da BBB
 *
 *******************************************************/

#include <stdint.h>

#include "hw_types.h"
#include "metronomo.h"
#include "soc_AM335x.h"

// whach dog register
#define WDT_BASE 0x44E35000
#define WDT_WSPR 0x48
#define WDT_WWPS 0x34

// whach dog timmer
#define SOC_WDT_1_REGS (0x44E35000)
#define WDT_WWPS (SOC_WDT_1_REGS + 0x34)  // Registrador de Status de Escrita
#define WDT_WLDR (SOC_WDT_1_REGS + 0x2C)  // Registrador de Carga (Tempo)
#define WDT_WTGR (SOC_WDT_1_REGS + 0x30)  // Registrador de Gatilho (O Petisco)
#define WDT_WSPR (SOC_WDT_1_REGS + 0x48)

// Offsets internos
#define DMTIMER_IRQSTATUS (0x28)
#define DMTIMER_IRQENABLE_SET (0x2C)
#define DMTIMER_IRQENABLE_CLR (0x30)  // desabilita geração de interupção
#define DMTIMER_TCLR (0x38)           // Registradores de controle
#define DMTIMER_TCRR (0x3C)           // contador
#define DMTIMER_TLDR (0x40)           // recarga
#define DMTIMER_TWPS (0x48)           // status
#define DMTIMER_TSICR (0x54)          // controle modo posted

// Endereços específicos do Timer 7
#define PRCM_TIMER7_CLKCTRL (0x44E00000 + 0x7C)
#define DPLL_TIMER7_CLKSEL (0x44E00500 + 0x04)

void timerSetup(void);

void DESLIGAR_WATCHDOG(void);

void ALIMENTAR_CAO_CONTROLADO(void);
// Trava a CPU pelo tempo em milissegundos
void mtimerDelay(uint32_t ms);

void WDT_Espera(uint32_t mascara);

void enable_wdt(uint32_t segundos);

void feed_wdt(void);

#endif
