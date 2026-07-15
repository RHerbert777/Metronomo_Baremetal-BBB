#include "display.h"

/*******************************************************
 * _  _   ___
 * | || | | _ \    Arquivo: display.c
 * | __ | |   /    Autor: Herbert Renner (RHerbert777)
 * |_||_| |_|_\    Data: 15/07/2026
 *
 * Descrição: Implementação do display TM1637
 *******************************************************/

// TABELA DE TRADUÇÃO (Números de 0 a 9)
const uint8_t codigo_numeros[] = {
    0x3F,  // 0
    0x06,  // 1
    0x5B,  // 2
    0x4F,  // 3
    0x66,  // 4
    0x6D,  // 5
    0x7D,  // 6
    0x07,  // 7
    0x7F,  // 8
    0x6F   // 9
};

// Micro delay por garantia
void delay_micros() { for (volatile int i = 0; i < 500; i++); }

void TM1637_Init(void) {
  MUX_CONFIG(0x7, CLK_PIN);

  MUX_CONFIG(0x27, DIO_PIN);

  CONF_DIR(SOC_GPIO_1_REGS, TM1637_CLK_PIN, GPIO_OUTPUT);
  CONF_DIR(SOC_GPIO_1_REGS, TM1637_DIO_PIN, GPIO_OUTPUT);

  GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);
  GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_DIO_PIN);

  delay_micros();
  TM1637_DisplayOn(7);
}

void TM1637_Start(void) {
  GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);
  GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_DIO_PIN);
  delay_micros();

  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_DIO_PIN);  // DIO cai primeiro
  delay_micros();

  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);  // CLK cai depois
  delay_micros();
}

void TM1637_Stop(void) {
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_DIO_PIN);
  delay_micros();

  GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);  // CLK sobe primeiro
  delay_micros();

  GPIO_SET_PIN(SOC_GPIO_1_REGS,
               TM1637_DIO_PIN);  // DIO sobe depois, fechando o pacote
  delay_micros();
}

// Função interna para enviar 8 bits (1 Byte) para o display
void TM1637_Escrever_Byte(uint8_t dado) {
  // Laço para enviar os 8 bits, do menos significativo (LSB) para o mais (MSB)
  for (int i = 0; i < 8; i++) {
    // Abaixa o Relógio (Prepara para trocar o valor do fio de dados)
    GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);
    delay_micros();

    // Coloca 0 ou 1 no DIO, dependendo do último bit da variável 'dado'
    if (dado & 0x01) {
      GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_DIO_PIN);
    } else {
      GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_DIO_PIN);
    }
    delay_micros();

    // Sobe o Relógio (Dá o "tique" pro display ler o fio)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);
    delay_micros();

    // Empurra a variável para a direita para expor o próximo bit na próxima
    // volta
    dado = dado >> 1;
  }

  // 9º PULSO: Lendo o ACK (Acknowledge)
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);

  // Transforma o nosso pino DIO temporariamente em ENTRADA para ouvir o display
  CONF_DIR(SOC_GPIO_1_REGS, TM1637_DIO_PIN, GPIO_INPUT);
  delay_micros();

  // Sobe o clock (O display joga o DIO para GND neste momento se entendeu a
  // mensagem)
  GPIO_SET_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);
  delay_micros();

  // Abaixa o clock para finalizar o ciclo do ACK
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, TM1637_CLK_PIN);

  // Transforma o DIO de volta em SAÍDA para a BeagleBone voltar a comandar
  CONF_DIR(SOC_GPIO_1_REGS, TM1637_DIO_PIN, GPIO_OUTPUT);
  delay_micros();
}

void TM1637_Escrever_Segmentos_Brutos(uint8_t posicao, uint8_t dados_brutos) {
  TM1637_Start();
  TM1637_Escrever_Byte(0x44);
  TM1637_Stop();

  TM1637_Start();
  TM1637_Escrever_Byte(0xC0 + posicao);
  TM1637_Escrever_Byte(dados_brutos);
  TM1637_Stop();
}

void TM1637_Escrever_Digito(uint8_t posicao, uint8_t numero) {
  uint8_t codigo_segmento;

  if (numero <= 9) {
    codigo_segmento = codigo_numeros[numero];
  } else {
    codigo_segmento = 0x00;
  }

  TM1637_Start();
  TM1637_Escrever_Byte(TM1637_CMD_FIXED_ADDR);
  TM1637_Stop();

  TM1637_Start();
  TM1637_Escrever_Byte(posicao);
  TM1637_Escrever_Byte(codigo_segmento);
  TM1637_Stop();

  TM1637_DisplayOn(7);
}

void TM1637_Limpar_Tela(void) {
  TM1637_Start();
  TM1637_Escrever_Byte(0x40);
  TM1637_Stop();

  TM1637_Start();
  TM1637_Escrever_Byte(0xC0);

  TM1637_Escrever_Byte(0x00);  // Apaga o 1º dígito
  TM1637_Escrever_Byte(0x00);  // Apaga o 2º dígito
  TM1637_Escrever_Byte(0x00);  // Apaga o 3º dígito
  TM1637_Escrever_Byte(0x00);  // Apaga o 4º dígito

  TM1637_Stop();
}

void TM1637_DisplayOff(void) {
  TM1637_Start();
  TM1637_Escrever_Byte(0x80);
  TM1637_Stop();
}

void TM1637_DisplayOn(uint8_t brilho) {
  // Trava de segurança: garante que o brilho não passe de 7
  if (brilho > 7) brilho = 7;

  TM1637_Start();
  // O comando base para ligar é 0x88. Somando o brilho, vamos de 0x88 a 0x8F
  TM1637_Escrever_Byte(0x88 + brilho);
  TM1637_Stop();
}

//! Logica para display invertido
//! =======================================================

uint8_t TM1637_Inverter_Segmentos(uint8_t segmentos) {
  uint8_t invertido = 0;

  if (segmentos & 0x01) invertido |= 0x08;  // Segmento A vira D
  if (segmentos & 0x02) invertido |= 0x10;  // Segmento B vira E
  if (segmentos & 0x04) invertido |= 0x20;  // Segmento C vira F
  if (segmentos & 0x08) invertido |= 0x01;  // Segmento D vira A
  if (segmentos & 0x10) invertido |= 0x02;  // Segmento E vira B
  if (segmentos & 0x20) invertido |= 0x04;  // Segmento F vira C
  if (segmentos & 0x40) invertido |= 0x40;  // Segmento G (Meio) continua G

  return invertido;
}

void TM1637_Escrever_Digito_Invertido(uint8_t posicao_logica, uint8_t valor) {
  uint8_t desenho_original = codigo_numeros[valor];
  uint8_t desenho_invertido = TM1637_Inverter_Segmentos(desenho_original);

  uint8_t posicao_fisica;
  if (posicao_logica == TM1637_POS_1)
    posicao_fisica = TM1637_POS_4;
  else if (posicao_logica == TM1637_POS_2)
    posicao_fisica = TM1637_POS_3;
  else if (posicao_logica == TM1637_POS_3)
    posicao_fisica = TM1637_POS_2;
  else if (posicao_logica == TM1637_POS_4)
    posicao_fisica = TM1637_POS_1;
  else
    posicao_fisica = posicao_logica;

  TM1637_Start();
  TM1637_Escrever_Byte(posicao_fisica);
  TM1637_Escrever_Byte(desenho_invertido);
  TM1637_Stop();
}

void TM1637_Escrever_Segmentos_Brutos_Invertido(uint8_t posicao,
                                                uint8_t segmentos_brutos) {
  uint8_t segmentos_corrigidos = TM1637_Inverter_Segmentos(segmentos_brutos);

  uint8_t posicao_fisica = 3 - posicao;

  TM1637_Escrever_Segmentos_Brutos(posicao_fisica, segmentos_corrigidos);
}