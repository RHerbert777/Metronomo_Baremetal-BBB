#ifndef GPIO_H
#define GPIO_H

/*******************************************************
 * _  _   ___  
 * | || | | _ \    Arquivo: gpio.h
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: Modulo gpio
 *******************************************************/

#include <stdint.h>

#include "hw_types.h"
#include "metronomo.h"
#include "soc_AM335x.h"

// CLOCK
#define CKM_PER_GPIO1_CLKCTRL (0x0AC)
#define CKM_PER_GPIO2_CLKCTRL (0xB0)

// DIREÇÃO
#define GPIO_OE               (0x134)
#define GPIO_DATAIN           (0x138)
#define GPIO_DATAOUT          (0x13C)
#define GPIO_OUTPUT           (0x000)
#define GPIO_INPUT            (0x001)

// Controle
#define GPIO_SETDATAOUT       (0x194)
#define GPIO_CLEANDATAOUT     (0X190)
#define GPIO_DEBOUNCENABLE    (0x150)   // Liga o filtro em um pino específico
#define GPIO_DEBOUNCINGTIME   (0x154)  // Define o tempo do filtro (0 a 255)

// Seta o CLock do modulo em PRCM
void AT_MOD(unsigned int MODULO);

// Recebe o pino e a direção entrada e ou saida e a base do gpio que vamos setar
void CONF_DIR(unsigned int GPIO_BASE, uint32_t pin, uint32_t dir);

// Seta o pin como 1 (alto)
void GPIO_SET_PIN(unsigned int GPIO_BASE, uint32_t pin);

// Seta o pin como 0 (baixo)
void GPIO_CLEAN_PIN(unsigned int GPIO_BASE, uint32_t pin);

// Seta a energia para o periferico de debounce
void PRCM_Debounce_Enable(unsigned int offset_clkctrl);

// Seta o modulo pino e tempo do debounce aceitando ate 255 que daria por volta
// de 8milisegunodo Sete o maior tempo posivel 255
void gpioDebounceSetup(uint32_t modulo_gpio, uint8_t pino, uint8_t tempo_debounce);

// Seta a multiplexação do pino, recebe MODE = 0-7 e Modulo a ser setado
void MUX_CONFIG(uint32_t CONFIG, unsigned int MODULO);

// Faz a leitura do pino de GPIO em modo entrada
uint32_t GPIO_READ_PIN(unsigned int GPIO_BASE, uint32_t pin);
#endif