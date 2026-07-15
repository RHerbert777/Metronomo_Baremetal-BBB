#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#include "gpio.h"
#include "soc_AM335x.h"

// Pinos definidos DIO e CLK
#define CLK_PIN CM_conf_gpmc_a3
#define DIO_PIN CM_conf_gpmc_a1

#define TM1637_CLK_PIN 19  // GPIO1_19 (P9_16)
#define TM1637_DIO_PIN 17  // GPIO1_17 (P9_23)

// CONSTANTES DO DISPLAY
#define TM1637_CMD_FIXED_ADDR \
  0x44  // Comando: Escrever em um endereço específico
#define TM1637_CMD_DISPLAY_ON \
  0x8F  // Comando: Ligar tela com brilho máximo (88 a 8F)

#define TM1637_POS_1 (0xC0)  // Endereço do 1º dígito (Esquerda)
#define TM1637_POS_2 (0xC1)  // Endereço do 2º dígito
#define TM1637_POS_3 (0xC2)  // Endereço do 3º dígito
#define TM1637_POS_4 (0xC3)  // Endereço do 4º dígito (Direita)

#define SEG_A (0x01)  // Índice 0: Segmento A (Topo)
#define SEG_B (0x02)  // Índice 1: Segmento B (Superior Direito)
#define SEG_C (0x04)  // Índice 2: Segmento C (Inferior Direito)
#define SEG_D (0x08)  // Índice 3: Segmento D (Base)
#define SEG_E (0x10)  // Índice 4: Segmento E (Inferior Esquerdo)
#define SEG_F (0x20)  // Índice 5: Segmento F (Superior Esquerdo)
#define SEG_G (0x40)  // Índice 6: Segmento G (Meio)
#define SEG_P (0x80)  // Índice 7: Ponto Decimal (DP)

// Inicializa os pinos GPIO e liga a tela do display
void TM1637_Init(void);

// Escreve um número (0 a 9) em uma posição específica (TM1637_POS_1 a POS_4)
void TM1637_Escrever_Digito(uint8_t posicao, uint8_t numero);

// Limpa toda a tela (apaga os 4 dígitos)
void TM1637_Limpar_Tela(void);

// Escreve segmentos brutos de dados no display
void TM1637_Escrever_Segmentos_Brutos(uint8_t posicao, uint8_t dados_brutos);

// Desliga completamente o brilho do display
void TM1637_DisplayOff(void);

// Liga o display e define o brilho (0 = Mínimo, 7 = Máximo)
void TM1637_DisplayOn(uint8_t brilho);

uint8_t TM1637_Inverter_Segmentos(uint8_t segmentos);

void TM1637_Escrever_Digito_Invertido(uint8_t posicao_logica, uint8_t valor);

void TM1637_Escrever_Segmentos_Brutos_Invertido(uint8_t posicao,
                                                uint8_t segmentos_brutos);

#endif
