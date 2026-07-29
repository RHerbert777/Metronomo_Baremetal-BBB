#include "metronomo.h"

/*******************************************************
 ** _  _   ___
 ** | || | | _ \    Arquivo: main.c
 ** | __ | |   /    Autor: Herbert Renner (RHerbert777)
 ** |_||_| |_|_\    Data: 15/07/2026
 **
 ** Descrição: Implementação de um metronomo utilizando por musicos para a
 * contagem de tempos e compassos desenvolvido
 ** na disciplina de Tecnicas de programação para sistemas embarcados - (TPSE1)
 * do professor Francisco Helder Candido dos Santos Filho 
 ** utilizando a beagle bone black em bare metal.
 *******************************************************/

// Tempos e compasos
volatile uint16_t bpm = 120;
volatile uint8_t compasso = 4;
volatile uint8_t batida_atual = 1;
volatile uint32_t intervalo_ms = 500;
volatile uint32_t tempo_ultimo_compasso = 0;
volatile uint32_t tempo_inicio_bipe = 0;

// Flags do Maestro
volatile bool ordem_TOQUE = false;
volatile bool ordem_CORTE = false;
volatile bool som_esta_ativo = false;
volatile EstadoMetronomo estado_metronomo = STANDBY;

// flag modo visual
volatile uint8_t modo_visual = 1;
volatile uint8_t flag_secreta = 0;

int main() {
  uartSetup();
  
  start();
  
  // Modo de apresentação e testes
  Dispaly_test();
  LEDs_test();
  LED_SIMPLES_ON();

  //*Standby
  while (1) {
    if (flag_start == true) {
      flag_start = false;
      break;
    }
  }

  TM1637_Limpar_Tela();

  APAGAR_TODOS_LEDS_COLUNA();

  LED_SIMPLES_OFF();

  SET_BPM();

  while (1) {
    // Interupção de set
    if (flag_SET == true) {
      Pulso_buzzer();
      SET_BPM();
    }

    // START / STOP =======================================
    if (flag_start == true) {
      flag_start = false;

      if (estado_metronomo == STANDBY) {
        estado_metronomo = TOCANDO;
        tempo_ultimo_compasso = sistema_ms - intervalo_ms;
        batida_atual = 1;
      } else {
        estado_metronomo = STANDBY;
        ordem_CORTE = true;
      }
    }

    // MAESTRO - Gerador de eventos ========================
    if (estado_metronomo == TOCANDO) {
      if ((sistema_ms - tempo_ultimo_compasso) >= intervalo_ms) {
        tempo_ultimo_compasso = sistema_ms;
        tempo_inicio_bipe = sistema_ms;

        som_esta_ativo = true;
        ordem_TOQUE = true;
      }
      // Momento corte
      if (som_esta_ativo == true) {
        uint32_t diferenca = sistema_ms - tempo_inicio_bipe;

        if (diferenca >= 50) {
          som_esta_ativo = false;
          ordem_CORTE = true;
        }
      }
    }

    // BUZZER
    if (ordem_TOQUE == true) {
      if (batida_atual == 1) {
        PWM_SET_FREQUENCY(1000);
      } else {
        PWM_SET_FREQUENCY(500);
      }

      PWM_PLAY();
    }
    if (ordem_CORTE == true) {
      PWM_STOP();
    }

    // MODO SIMPLES (1 ou 3)
    if (modo_visual == 1 || modo_visual == 3) {
      if (ordem_TOQUE == true) LED_SIMPLES_ON();
      if (ordem_CORTE == true) LED_SIMPLES_OFF();
    }

    // MODO PILHA ANIMADA (2 ou 3)
    if (modo_visual == 2 || modo_visual == 3) {
      if (estado_metronomo == TOCANDO) {
        Processar_Animacao_Coluna();
      } else {
        APAGAR_TODOS_LEDS_COLUNA();
      }
    }

    // Limpeza de flags e atualização de batidas
    if (ordem_TOQUE == true) {
      batida_atual++;
      if (batida_atual > compasso) {
        batida_atual = 1;
      }
      ordem_TOQUE = false;
    }

    if (ordem_CORTE == true) {
      ordem_CORTE = false;
    }

    // Musica secreta
    if (estado_metronomo == STANDBY) {
      if (flag_UP == true) {
        flag_UP = false;
        Musica_secreta();
      }
    }
  }
  return 0;
}