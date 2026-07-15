#include "gpio.h"

/*******************************************************
 * _  _   ___
 * | || | | _ \    Arquivo: gpio.c
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição:
 *
 *******************************************************/

void AT_MOD(unsigned int MODULO) {
  HWREG(SOC_CM_PER_REGS + MODULO) |= (0x2 << 0);

  while ((HWREG(SOC_CM_PER_REGS + MODULO) & (3 << 16)) != 0) {
  }
}

void PRCM_Debounce_Enable(unsigned int offset_clkctrl) {
  HWREG(SOC_CM_PER_REGS + offset_clkctrl) |= (1 << 18);

  while ((HWREG(SOC_CM_PER_REGS + offset_clkctrl) & (1 << 18)) == 0);
}

void gpioDebounceSetup(uint32_t modulo_gpio, uint8_t pino, uint8_t tempo_debounce) {
  HWREG(modulo_gpio + GPIO_DEBOUNCINGTIME) = tempo_debounce;
  HWREG(modulo_gpio + GPIO_DEBOUNCENABLE) |= (1 << pino);
}

void MUX_CONFIG(uint32_t CONFIG, unsigned int MODULO) {
  uint32_t temp = HWREG(SOC_CONTROL_REGS + MODULO);
  temp &= ~0x3F;
  temp |= (CONFIG & 0x3F);
  HWREG(SOC_CONTROL_REGS + MODULO) = temp;
}

void CONF_DIR(unsigned int GPIO_BASE, uint32_t pin, uint32_t dir) {
  unsigned int addr_temp = GPIO_BASE + GPIO_OE;
  unsigned int val_temp = HWREG(addr_temp);
  val_temp &= ~(1 << pin);
  val_temp |= (dir << pin);
  HWREG(addr_temp) = val_temp;
}

void GPIO_SET_PIN(unsigned int GPIO_BASE, uint32_t pin) {
  unsigned int addr_temp = GPIO_BASE + GPIO_SETDATAOUT;
  unsigned int val_temp = 1 << pin;
  HWREG(addr_temp) |= val_temp;
}

void GPIO_CLEAN_PIN(unsigned int GPIO_BASE, uint32_t pin) {
  unsigned int addr_temp = GPIO_BASE + GPIO_CLEANDATAOUT;
  unsigned int val_temp = 1 << pin;
  HWREG(addr_temp) = val_temp;  // altera o pino especifico
}

uint32_t GPIO_READ_PIN(unsigned int GPIO_BASE, uint32_t pin) {
  uint32_t estado = HWREG(GPIO_BASE + GPIO_DATAIN);

  if (estado & (1 << pin)) {
    return 1;
  } else {
    return 0;
  }
}