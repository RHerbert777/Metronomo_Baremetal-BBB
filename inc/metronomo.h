#ifndef METRONOMO_H
#define METRONOMO_H

#include <stdint.h>
#include "control_module.h"
#include "display.h"
#include "gpio.h"
#include "hw_types.h"
#include "interupt.h"
#include "pad.h"
#include "pwm.h"
#include "soc_AM335x.h"
#include "timer.h"
#include "tools.h"
#include "uart.h"

typedef enum {
    STANDBY,
    TOCANDO
} EstadoMetronomo;

extern volatile EstadoMetronomo estado_metronomo;

//flags e gerador de eventos
// ==========================================
// VARIÁVEIS MUSICAIS
// ==========================================
extern volatile uint16_t bpm;
extern volatile uint8_t compasso;
extern volatile uint8_t batida_atual;
extern volatile uint32_t intervalo_ms;
// ==========================================
// RELÓGIOS DO SISTEMA
// ==========================================
extern volatile uint32_t sistema_ms;             // Atualizado pelo seu Timer 7
extern volatile uint32_t tempo_ultimo_compasso;  
extern volatile uint32_t tempo_inicio_bipe;      

// ==========================================
// AS FLAGS DO MAESTRO (Eventos)
// ==========================================
extern volatile bool ordem_TOQUE;
extern volatile bool ordem_CORTE;
extern volatile bool som_esta_ativo;

extern volatile uint8_t modo_visual;

#define CM_CONF_GPMC_A2 (0x848) //configuração necesaria para o mux do buzzer

#endif