#include "timer.h"

/*******************************************************
 * _  _   ___  
 * | || | | _ \    Arquivo: timer.c
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: Implementação do timer da BBB
 *******************************************************/

void WDT_Espera(uint32_t mascara) { while (HWREG(WDT_WWPS) & mascara); }

void enable_wdt(uint32_t segundos) {
  uint32_t ciclos = segundos * 32768;  // 32.768 Hz

  uint32_t valor_carga = 0xFFFFFFFF - ciclos + 1;

  WDT_Espera(0x04);
  HWREG(WDT_WLDR) = valor_carga;

  WDT_Espera(0x10);
  HWREG(WDT_WSPR) = 0xBBBB;

  WDT_Espera(0x10);
  HWREG(WDT_WSPR) = 0x4444;
}

// função que não deixa o whatch dog resetar
void feed_wdt(void) {
  static uint32_t petisco = 0;  // Guarda o último valor dado
  petisco++;

  WDT_Espera(0x08);           // Espera
  HWREG(WDT_WTGR) = petisco;  // Entrega o petisco ao Cão!
}

void DESLIGAR_WATCHDOG(void) {
  HWREG(WDT_BASE + WDT_WSPR) = 0xAAAA;

  while ((HWREG(WDT_BASE + WDT_WWPS) & (1 << 4)) != 0);

  HWREG(WDT_BASE + WDT_WSPR) = 0x5555;

  while ((HWREG(WDT_BASE + WDT_WWPS) & (1 << 4)) != 0);
}

// Alimenta o whach dog de forma controlada
void ALIMENTAR_CAO_CONTROLADO(void) {
  static uint32_t tempo_ultima_refeicao = 0;

  if ((sistema_ms - tempo_ultima_refeicao) >= 1000) {
    feed_wdt();

    tempo_ultima_refeicao = sistema_ms;  // Atualiza a hora da última refeição
  }
}

void timerSetup(void) {
  // PRCM
  HWREG(PRCM_TIMER7_CLKCTRL) |= 0x02;
  while ((HWREG(PRCM_TIMER7_CLKCTRL) & 0x03) != 0x02);

  HWREG(DPLL_TIMER7_CLKSEL) = 0x01;  // conecta ao cristal de 24mhz

  // posted libera a cpu para executar outras tarefas
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TSICR) |= (1 << 2);

  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) & (1 << 0));
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCLR) &= ~(1 << 0);

  // zera o contador
  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) & (1 << 1));
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCRR) = 0;

  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) & (1 << 2));
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TLDR) = 0;
}

void mtimerDelay(uint32_t ms) {
  // 1 milissegundo = 24.000 ticks a 24 MHz
  uint32_t ticks_needed = ms * 24000;

  // Zera o contador
  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) & (1 << 1));
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCRR) = 0;

  // Dá o Start
  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) & (1 << 0));
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCLR) |= (1 << 0);

  // Fica lendo até atingir o tempo
  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCRR) < ticks_needed);

  // Dá o Stop
  while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) & (1 << 0));
  HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCLR) &= ~(1 << 0);
}