#include "pwm.h"

/*******************************************************
 * _  _   ___
 * | || | | _ \    Arquivo: pwm.c
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: Implementação do modulo de pwm
 *******************************************************/

void PRCM_ENABLE_MODE(uint32_t module_offset) {
  unsigned int addr_prcm = SOC_CM_PER_REGS + module_offset;

  HWREG(addr_prcm) |= 0x2;

  while ((HWREG(addr_prcm) & 0x30000) != 0);
  return;
}

void SET_BUZZER(uint32_t module_offset) {
  // SYSCONFIG -------------------------------------------------------------
  unsigned int addr_temp = SOC_PWMSS1_REGS + PWMSS1_SYSCONFIG;
  unsigned int val_temp = HWREG(addr_temp);

  val_temp &= ~0x3E;  // limpando os 5 bits que iremos configurar

  val_temp |= (1 << 4);  // STANDBYMODE = 1h (No-standby)
  val_temp |= (1 << 2);  // IDLEMODE = 1h (No-idle)
  val_temp |= (1 << 1);  // FREEEMU = 1h (Free Emulation)

  HWREG(addr_temp) = val_temp;

  // SYSCLOCK ----------------------------------------------------------------
  addr_temp = SOC_PWMSS1_REGS + PWMSS1_CLKCONFIG;
  val_temp = HWREG(addr_temp);

  // ligando o modulo ePWMCLK_EN
  val_temp |= (1 << 8);

  HWREG(addr_temp) = val_temp;

  // SYSSTATUS ---------------------------------------------------------------
  addr_temp = SOC_PWMSS1_REGS + PWMSS1_CLKSTATUS;  // Offset 0x0C
  while ((HWREG(addr_temp) & (1 << 8)) == 0);
  // Aguarda o Bit 8 virar 1 (Ack do hardware)

  HWREG(SOC_CONTROL_REGS + PWMSS_CTRL) |= (1 << 1);  // Liberando a trava

  // Configura a regra da onda:
  // Sobe para 3.3V quando zerar, cai para 0V quando empatar com o CMPA
  HWREGH(SOC_EHRPWM1_REGS + AQCTLA) = 0x0012;

  // Configura o contador para o modo Crescente (Up-Count)
  HWREGH(SOC_EHRPWM1_REGS + TBCTL) = 0x1C00;

  // Inicia o sistema em completo silêncio
  PWM_STOP();
}

void PWM_SET_FREQUENCY(uint32_t freq_hz) {
  if (freq_hz == 0) {
    return;
  }

  // Calculo Teto
  uint32_t valor_tbprd = PWM_CLOCK_HZ / freq_hz;

  // Set frequencia
  HWREGH(SOC_EHRPWM1_REGS + TBPRD) = (unsigned short)valor_tbprd;

  // Conf DUTY CYCLE
  HWREGH(SOC_EHRPWM1_REGS + CMPA) = (unsigned short)(valor_tbprd / 20);

  // Inicio TBCTR zerado
  HWREGH(SOC_EHRPWM1_REGS + TBCTR) = 0x0000;
}

void PWM_PLAY() {
  unsigned short periodo_atual = HWREGH(SOC_EHRPWM1_REGS + TBPRD);
  HWREGH(SOC_EHRPWM1_REGS + CMPA) = periodo_atual / 20;
  return;
}

void PWM_STOP() {
  // Zera a largura do pulso (Duty Cycle = 0%)
  HWREGH(SOC_EHRPWM1_REGS + CMPA) = 0;
  return;
}

void PWM_ENABLE(uint8_t estado) {
  if (estado == 1) {
    unsigned short periodo_atual = HWREGH(SOC_EHRPWM1_REGS + TBPRD);

    HWREGH(SOC_EHRPWM1_REGS + CMPA) = periodo_atual / 20;
  } else if (estado == 0) {
    // Silenciar: Zera o Duty Cycle suavemente
    HWREGH(SOC_EHRPWM1_REGS + CMPA) = 0;
  }
}