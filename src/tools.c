#include "metronomo.h"

void Pulso_buzzer() {
  uint32_t pulso = sistema_ms;

  PWM_SET_FREQUENCY(440);
  PWM_PLAY();

  while ((sistema_ms - pulso) <= 50) {
    // aguarda
  }

  PWM_STOP();
}

// teste do display 8888
void Dispaly_test() {
  TM1637_Escrever_Digito_Invertido(TM1637_POS_1, 8);

  TM1637_Escrever_Digito_Invertido(TM1637_POS_2, 8);

  TM1637_Escrever_Digito_Invertido(TM1637_POS_3, 8);

  TM1637_Escrever_Digito_Invertido(TM1637_POS_4, 8);
}

// Acender todos os leds
void LEDs_test() {
  GPIO_SET_PIN(SOC_GPIO_1_REGS, 6);
  GPIO_SET_PIN(SOC_GPIO_1_REGS, 2);
  GPIO_SET_PIN(SOC_GPIO_1_REGS, 14);
  GPIO_SET_PIN(SOC_GPIO_1_REGS, 29);
  GPIO_SET_PIN(SOC_GPIO_1_REGS, 12);
}

// MENU SET_BPM =========================================================
void start() {
  // CAMADA 1: CLOCKS E ENERGIA (Ligar os disjuntores da casa)
  AT_MOD(CKM_PER_GPIO1_CLKCTRL);
  AT_MOD(CKM_PER_GPIO2_CLKCTRL);
  PRCM_Debounce_Enable(CKM_PER_GPIO1_CLKCTRL);  // Relógio de filtro dos botões
  PRCM_Debounce_Enable(CKM_PER_GPIO2_CLKCTRL);

  // MUX LEDs
  MUX_CONFIG(0x7, CM_conf_gpmc_ad6);
  CONF_DIR(SOC_GPIO_1_REGS, 6, GPIO_OUTPUT);

  MUX_CONFIG(0x7, CM_conf_gpmc_ad2);
  CONF_DIR(SOC_GPIO_1_REGS, 2, GPIO_OUTPUT);

  MUX_CONFIG(0x7, CM_conf_gpmc_ad14);
  CONF_DIR(SOC_GPIO_1_REGS, 14, GPIO_OUTPUT);

  MUX_CONFIG(0x7, CM_conf_gpmc_csn0);
  CONF_DIR(SOC_GPIO_1_REGS, 29, GPIO_OUTPUT);

  MUX_CONFIG(0x7, CM_conf_gpmc_ad12);
  CONF_DIR(SOC_GPIO_1_REGS, 12, GPIO_OUTPUT);

  MUX_CONFIG(0x7, CM_conf_gpmc_a0);
  CONF_DIR(SOC_GPIO_1_REGS, 16, GPIO_OUTPUT);

  // MUX BOTTONS
  MUX_CONFIG(0x27, CM_conf_gpmc_clk);
  CONF_DIR(SOC_GPIO_2_REGS, 1, GPIO_INPUT);  // SET

  MUX_CONFIG(0x27, CM_conf_gpmc_wen);
  CONF_DIR(SOC_GPIO_2_REGS, 4, GPIO_INPUT);  // UP

  // Novos pinos seguros para START e DOWN
  MUX_CONFIG(0x27, CM_conf_gpmc_ad15);
  CONF_DIR(SOC_GPIO_1_REGS, 15, GPIO_INPUT);  // START (P8.15)

  MUX_CONFIG(0x27, CM_conf_gpmc_ben1);
  CONF_DIR(SOC_GPIO_1_REGS, 28, GPIO_INPUT);  // DOWN (P9.12)

  // uartPutString("mux\r\n");

  // Periféricos Complexos
  APAGAR_TODOS_LEDS_COLUNA();  // inicializando apagado

  TM1637_Init();

  PRCM_ENABLE_MODE(CM_PER_EPWMSS1_CLKCTRL);  // ativamos o prcm

  HWREG(SOC_CONTROL_REGS + CM_CONF_GPMC_A2) = 6;  // Mux Buzzer
  SET_BUZZER(CM_PER_EPWMSS1_CLKCTRL);
  PWM_SET_FREQUENCY(440);

  // Interupçoes
  gpioDebounceSetup(SOC_GPIO_1_REGS, 15, 255);
  gpioInterruptSetup(SOC_GPIO_1_REGS, 15, EDGE_RISING);

  gpioDebounceSetup(SOC_GPIO_2_REGS, 1, 255);
  gpioInterruptSetup(SOC_GPIO_2_REGS, 1, EDGE_RISING);

  gpioDebounceSetup(SOC_GPIO_2_REGS, 4, 255);
  gpioInterruptSetup(SOC_GPIO_2_REGS, 4, EDGE_RISING);

  gpioDebounceSetup(SOC_GPIO_1_REGS, 28, 255);
  gpioInterruptSetup(SOC_GPIO_1_REGS, 28, EDGE_RISING);

  IntAINTCInit();  // Liga o cérebro das interrupções

  AINTC_Destrancar_Interrupcao(SOC_GPIO_1_REGS);
  AINTC_Destrancar_Interrupcao(SOC_GPIO_2_REGS);

  // Inicializa contagem de tempo
  Timer7_Start_System_Tick();

  DESLIGAR_WATCHDOG();
  // enable_wdt(5);
  return;
}

// VISUAL LEDS ====================================================
void LED_SIMPLES_ON() { GPIO_SET_PIN(SOC_GPIO_1_REGS, 16); }

void LED_SIMPLES_OFF() { GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, 16); }

// Limpamos a coluna de led
void APAGAR_TODOS_LEDS_COLUNA(void) {
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, 6);
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, 2);
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, 14);
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, 29);
  GPIO_CLEAN_PIN(SOC_GPIO_1_REGS, 12);
}

void ACENDER_LED_EXCLUSIVO(uint8_t led_ativo) {
  APAGAR_TODOS_LEDS_COLUNA();

  if (led_ativo == 1)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 6);  // base da pilha
  else if (led_ativo == 2)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 2);
  else if (led_ativo == 3)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 14);
  else if (led_ativo == 4)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 29);
  else if (led_ativo == 5)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 12);  // topo da pilha
}

void LIGAR_LED(uint8_t led_ativo) {
  if (led_ativo == 1)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 6);  // base da pilha
  else if (led_ativo == 2)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 2);
  else if (led_ativo == 3)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 14);
  else if (led_ativo == 4)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 29);
  else if (led_ativo == 5)
    GPIO_SET_PIN(SOC_GPIO_1_REGS, 12);  // topo da pilha
}

// Processa a animação da coluna de leds
void Processar_Animacao_Coluna(void) {
  volatile uint32_t tempo_corrido = sistema_ms - tempo_ultimo_compasso;
  // Maquina de estados
  static uint8_t led_atual = 0;
  uint8_t novo_led = 0;

  // Garantimos o pulso de 50ms no ultimo led
  if (tempo_corrido < 50) {
    novo_led = 5;
  }

  else {
    volatile uint32_t tempo_por_passo = intervalo_ms / 4;

    if (tempo_corrido < (tempo_por_passo * 1))
      novo_led = 1;
    else if (tempo_corrido < (tempo_por_passo * 2))
      novo_led = 2;
    else if (tempo_corrido < (tempo_por_passo * 3))
      novo_led = 3;
    else
      novo_led = 4;
  }

  // Garentimos que so ira trocar se ouver mudanças
  if (novo_led != led_atual) {
    ACENDER_LED_EXCLUSIVO(novo_led);
    led_atual = novo_led;
  }
}

// Apresenta o valor de BPM no formato | 120|
void DISPLAY_CONVERSAO_BPM(uint16_t valor_bpm) {
  TM1637_Limpar_Tela();

  uint8_t centenas = (valor_bpm / 100) % 10;
  uint8_t dezenas = (valor_bpm / 10) % 10;
  uint8_t unidades = valor_bpm % 10;

  if (centenas == 0) {
    TM1637_Escrever_Digito_Invertido(TM1637_POS_2, 10);  // Apagamos a centena
  } else {
    TM1637_Escrever_Digito_Invertido(TM1637_POS_2, centenas);
  }

  TM1637_Escrever_Digito_Invertido(TM1637_POS_3, dezenas);
  TM1637_Escrever_Digito_Invertido(TM1637_POS_4, unidades);
}

// Apresenta no display no formato |F - - 1|
void DISPLAY_TELA_VISUAL(uint8_t modo) {
  TM1637_Limpar_Tela();

  TM1637_Escrever_Segmentos_Brutos_Invertido(0, 0x71);

  TM1637_Escrever_Segmentos_Brutos_Invertido(1, 0x40);

  TM1637_Escrever_Segmentos_Brutos_Invertido(2, 0x40);

  TM1637_Escrever_Digito_Invertido(3, modo);
}

// Apresenta o display para o modo compasso: |C - - 4|
void DISPLAY_CONVERSAO_COMPASSO(uint8_t compasso) {
  TM1637_Limpar_Tela();

  TM1637_Escrever_Segmentos_Brutos_Invertido(0, 0x39);

  TM1637_Escrever_Segmentos_Brutos_Invertido(1, 0x40);

  TM1637_Escrever_Segmentos_Brutos_Invertido(2, 0x40);

  TM1637_Escrever_Digito_Invertido(3, compasso);
}

// Interupção que seta o bpm e o compaço do metronomo
void SET_BPM() {
  PWM_STOP();
  flag_SET = false;
  flag_UP = false;
  flag_DOWN = false;
  flag_start = false;

  uint8_t tela_edicao = 0;

  DISPLAY_CONVERSAO_BPM(bpm);  // Display recebe bpm atual

  while (1) {
    // BOTÃO SET: Troca de Tela
    if (flag_SET == true) {
      flag_SET = false;
      Pulso_buzzer();
      tela_edicao++;
      if (tela_edicao > 2) tela_edicao = 0;  // limitando telas

      if (tela_edicao == 0)
        DISPLAY_CONVERSAO_BPM(bpm);
      else if (tela_edicao == 1)
        DISPLAY_CONVERSAO_COMPASSO(compasso);
      else if (tela_edicao == 2)
        DISPLAY_TELA_VISUAL(modo_visual);
    }

    // TELA 0: Configuração de BPM
    if (tela_edicao == 0) {
      if (flag_UP == true) {
        flag_UP = false;
        Pulso_buzzer();
        bpm++;
        if (bpm > 300) bpm = 40;
        DISPLAY_CONVERSAO_BPM(bpm);
      }
      if (flag_DOWN == true) {
        flag_DOWN = false;
        Pulso_buzzer();
        if (bpm <= 40)
          bpm = 300;
        else
          bpm--;
        DISPLAY_CONVERSAO_BPM(bpm);
      }
    }

    // TELA 1: Configuração de COMPASSO
    if (tela_edicao == 1) {
      if (flag_UP == true) {
        flag_UP = false;
        Pulso_buzzer();
        compasso++;
        if (compasso > 7) compasso = 1;
        DISPLAY_CONVERSAO_COMPASSO(compasso);
      }
      if (flag_DOWN == true) {
        flag_DOWN = false;
        Pulso_buzzer();
        if (compasso <= 1)
          compasso = 7;
        else
          compasso--;
        DISPLAY_CONVERSAO_COMPASSO(compasso);
      }
    }

    // TELA 2: MODO VISUAL LED
    if (tela_edicao == 2) {
      if (flag_UP == true) {
        flag_UP = false;
        Pulso_buzzer();
        modo_visual++;
        if (modo_visual > 3) modo_visual = 1;
        DISPLAY_TELA_VISUAL(modo_visual);
      }
      if (flag_DOWN == true) {
        flag_DOWN = false;
        Pulso_buzzer();
        if (modo_visual <= 1)
          modo_visual = 3;
        else
          modo_visual--;
        DISPLAY_TELA_VISUAL(modo_visual);
      }
    }

    // BOTÃO START: Sair e Salvar
    if (flag_start == true) {
      flag_start = false;
      Pulso_buzzer();

      batida_atual = 1;
      intervalo_ms = 60000 / bpm;
      tempo_ultimo_compasso = sistema_ms - intervalo_ms;
      estado_metronomo = TOCANDO;

      TM1637_Limpar_Tela();

      DISPLAY_CONVERSAO_BPM(bpm);

      return;
    }
  }
}
//?
//==================================================================================
//! MUSIC
//?
//==================================================================================

void display_animacao_circular(unsigned int duracao_compasso_ms) {
  // Variáveis 'static' guardam o valor mesmo depois que a função termina
  static uint32_t ultimo_tempo_animacao = 0;
  static uint8_t estado_display = 0;

  // O seu rascunho estava no caminho certo! Aqui fazemos a checagem
  // não-bloqueante:
  if ((sistema_ms - ultimo_tempo_animacao) >= duracao_compasso_ms) {
    TM1637_Limpar_Tela();

    // Bateu o tempo! Reseta o cronômetro para o próximo compasso
    ultimo_tempo_animacao = sistema_ms;

    uint8_t segmentos = 0;

    // Máquina de estados do giro horário
    switch (estado_display) {
      case 0:
        segmentos = SEG_A;
        break;  // Topo
      case 1:
        segmentos = SEG_B;
        break;  // Superior Direito
      case 2:
        segmentos = SEG_C;
        break;  // Inferior Direito
      case 3:
        segmentos = SEG_D;
        break;  // Base
      case 4:
        segmentos = SEG_E;
        break;  // Inferior Esquerdo
      case 5:
        segmentos = SEG_F;
        break;  // Superior Esquerdo
    }

    // Escreve o mesmo padrão nos 4 dígitos do TM1637 de uma vez
    TM1637_Escrever_Segmentos_Brutos_Invertido(0, segmentos);
    TM1637_Escrever_Segmentos_Brutos_Invertido(1, segmentos);
    TM1637_Escrever_Segmentos_Brutos_Invertido(2, segmentos);
    TM1637_Escrever_Segmentos_Brutos_Invertido(3, segmentos);

    // Avança para o próximo quadro
    estado_display++;

    // Como temos 6 segmentos na borda, o reset ocorre depois do 5
    if (estado_display > 5) {
      estado_display = 0;
    }
  }
}

void Animacao_Leds_Torre(unsigned int duracao_compasso_ms) {
  static uint32_t ultimo_tempo_led = 0;

  // 0 = Estado Ímpar (1, 3, 5) | 1 = Estado Par (2, 4)
  static uint8_t estado_led = 0;

  // Verifica se bateu o tempo do compasso (Semínima)
  if ((sistema_ms - ultimo_tempo_led) >= duracao_compasso_ms) {
    // Atualiza o relógio para o próximo ciclo
    ultimo_tempo_led = sistema_ms;

    // Limpa a torre inteira (como você previu perfeitamente)
    APAGAR_TODOS_LEDS_COLUNA();

    // Máquina de alternância (Ping-Pong)
    if (estado_led == 0) {
      // == CASO ÍMPAR ==
      // (Use funções não-exclusivas para ligar vários ao mesmo tempo)
      LIGAR_LED(1);
      LIGAR_LED(3);
      LIGAR_LED(5);

      // Muda o estado para a próxima batida
      estado_led = 1;
      LED_SIMPLES_ON();
    } else if (estado_led == 1) {
      // == CASO PAR ==
      LIGAR_LED(2);
      LIGAR_LED(4);

      // Retorna ao estado inicial
      estado_led = 0;
      LED_SIMPLES_OFF();
    }
  }
}

const int melody[] = {
    NOTE_C5,  4,  // 1
    NOTE_F5,  4, NOTE_F5,  8, NOTE_G5,  8, NOTE_F5, 8, NOTE_E5, 8, NOTE_D5,  4,
    NOTE_D5,  4, NOTE_D5,  4, NOTE_G5,  4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5,  8,
    NOTE_F5,  8, NOTE_E5,  4, NOTE_C5,  4, NOTE_C5, 4, NOTE_A5, 4, NOTE_A5,  8,
    NOTE_AS5, 8, NOTE_A5,  8, NOTE_G5,  8, NOTE_F5, 4, NOTE_D5, 4, NOTE_C5,  8,
    NOTE_C5,  8, NOTE_D5,  4, NOTE_G5,  4, NOTE_E5, 4,

    NOTE_F5,  2, NOTE_C5,  4,  // 8
    NOTE_F5,  4, NOTE_F5,  8, NOTE_G5,  8, NOTE_F5, 8, NOTE_E5, 8, NOTE_D5,  4,
    NOTE_D5,  4, NOTE_D5,  4, NOTE_G5,  4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5,  8,
    NOTE_F5,  8, NOTE_E5,  4, NOTE_C5,  4, NOTE_C5, 4, NOTE_A5, 4, NOTE_A5,  8,
    NOTE_AS5, 8, NOTE_A5,  8, NOTE_G5,  8, NOTE_F5, 4, NOTE_D5, 4, NOTE_C5,  8,
    NOTE_C5,  8, NOTE_D5,  4, NOTE_G5,  4, NOTE_E5, 4, NOTE_F5, 2, NOTE_C5,  4,

    NOTE_F5,  4, NOTE_F5,  4, NOTE_F5,  4,  // 17
    NOTE_E5,  2, NOTE_E5,  4, NOTE_F5,  4, NOTE_E5, 4, NOTE_D5, 4, NOTE_C5,  2,
    NOTE_A5,  4, NOTE_AS5, 4, NOTE_A5,  4, NOTE_G5, 4, NOTE_C6, 4, NOTE_C5,  4,
    NOTE_C5,  8, NOTE_C5,  8, NOTE_D5,  4, NOTE_G5, 4, NOTE_E5, 4, NOTE_F5,  2,
    NOTE_C5,  4, NOTE_F5,  4, NOTE_F5,  8, NOTE_G5, 8, NOTE_F5, 8, NOTE_E5,  8,
    NOTE_D5,  4, NOTE_D5,  4, NOTE_D5,  4,

    NOTE_G5,  4, NOTE_G5,  8, NOTE_A5,  8, NOTE_G5, 8, NOTE_F5, 8,  // 27
    NOTE_E5,  4, NOTE_C5,  4, NOTE_C5,  4, NOTE_A5, 4, NOTE_A5, 8, NOTE_AS5, 8,
    NOTE_A5,  8, NOTE_G5,  8, NOTE_F5,  4, NOTE_D5, 4, NOTE_C5, 8, NOTE_C5,  8,
    NOTE_D5,  4, NOTE_G5,  4, NOTE_E5,  4, NOTE_F5, 2, NOTE_C5, 4, NOTE_F5,  4,
    NOTE_F5,  4, NOTE_F5,  4, NOTE_E5,  2, NOTE_E5, 4, NOTE_F5, 4, NOTE_E5,  4,
    NOTE_D5,  4,

    NOTE_C5,  2, NOTE_A5,  4,  // 36
    NOTE_AS5, 4, NOTE_A5,  4, NOTE_G5,  4, NOTE_C6, 4, NOTE_C5, 4, NOTE_C5,  8,
    NOTE_C5,  8, NOTE_D5,  4, NOTE_G5,  4, NOTE_E5, 4, NOTE_F5, 2, NOTE_C5,  4,
    NOTE_F5,  4, NOTE_F5,  8, NOTE_G5,  8, NOTE_F5, 8, NOTE_E5, 8, NOTE_D5,  4,
    NOTE_D5,  4, NOTE_D5,  4, NOTE_G5,  4, NOTE_G5, 8, NOTE_A5, 8, NOTE_G5,  8,
    NOTE_F5,  8, NOTE_E5,  4, NOTE_C5,  4, NOTE_C5, 4,

    NOTE_A5,  4, NOTE_A5,  8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,  // 45
    NOTE_F5,  4, NOTE_D5,  4, NOTE_C5,  8, NOTE_C5, 8, NOTE_D5, 4, NOTE_G5,  4,
    NOTE_E5,  4, NOTE_F5,  2, NOTE_C5,  4, NOTE_F5, 4, NOTE_F5, 8, NOTE_G5,  8,
    NOTE_F5,  8, NOTE_E5,  8, NOTE_D5,  4, NOTE_D5, 4, NOTE_D5, 4, NOTE_G5,  4,
    NOTE_G5,  8, NOTE_A5,  8, NOTE_G5,  8, NOTE_F5, 8, NOTE_E5, 4, NOTE_C5,  4,
    NOTE_C5,  4,

    NOTE_A5,  4, NOTE_A5,  8, NOTE_AS5, 8, NOTE_A5, 8, NOTE_G5, 8,  // 53
    NOTE_F5,  4, NOTE_D5,  4, NOTE_C5,  8, NOTE_C5, 8, NOTE_D5, 4, NOTE_G5,  4,
    NOTE_E5,  4, NOTE_F5,  2, REST,     4};

#define LIGAR 1
#define DESLIGAR 0
volatile bool tocando_musica_secreta = false;
volatile unsigned int wholenote;
volatile unsigned int total_elementos;

void Gerenciar_Visuais_Assincronos(void) {
  static uint32_t ultimo_tempo_visual = 0;
  uint32_t tempo_seminima = wholenote / 4;

  // Bateu o tempo da semínima?
  if ((sistema_ms - ultimo_tempo_visual) >= tempo_seminima) {
    ultimo_tempo_visual = sistema_ms;

    // Chama as funções que você já fez! Elas já sabem o que fazer.
    display_animacao_circular(wholenote / 4);
    Animacao_Leds_Torre(wholenote / 4);
  }
}

void Gerenciar_Musica_Assincrona(void) {
  static int thisNote = 0;
  static uint8_t fase_atual = 0;  // 0 = Iniciar, 1 = Tocando, 2 = Pausa
  static uint32_t tempo_inicio_fase = 0;

  // Variáveis para guardar os tempos calculados para a nota atual
  static uint32_t tempo_tocando = 0;
  static uint32_t tempo_silencio = 0;

  if (fase_atual == 0) {
    int divider = melody[thisNote + 1];
    int noteDuration = 0;

    // 1. CÁLCULO DA DURAÇÃO (A sua matemática impecável)
    if (divider > 0) {
      noteDuration = wholenote / divider;
    } else if (divider < 0) {
      noteDuration = wholenote / (-divider);
      noteDuration += (noteDuration / 2);
    }

    tempo_tocando = (noteDuration * 9) / 10;
    tempo_silencio = noteDuration - tempo_tocando;
    uartPutString("Preparação\r\n");

    // 2. LIGA O SOM
    if (melody[thisNote] == 0) {
      PWM_ENABLE(DESLIGAR);
    } else {
      PWM_SET_FREQUENCY(melody[thisNote]);
      PWM_ENABLE(LIGAR);
    }
    uartPutString("ativando som\r\n");
    // 3. MUDA DE ESTADO
    tempo_inicio_fase = sistema_ms;  // Tira a "foto" do relógio agora
    fase_atual = 1;  // Avisa que no próximo ciclo, vamos apenas esperar o som
  }

  // ==========================================
  // FASE 1: ESPERANDO OS 90% (SOM)
  // ==========================================
  else if (fase_atual == 1) {
    if ((sistema_ms - tempo_inicio_fase) >= tempo_tocando) {
      // Deu o tempo! Corta o som.
      PWM_ENABLE(DESLIGAR);

      // Prepara para a próxima fase
      tempo_inicio_fase = sistema_ms;
      fase_atual = 2;
      uartPutString("Tempo do som\r\n");
    }
  }

  // ==========================================
  // FASE 2: ESPERANDO OS 10% (SILÊNCIO)
  // ==========================================
  else if (fase_atual == 2) {
    if ((sistema_ms - tempo_inicio_fase) >= tempo_silencio) {
      // O silêncio acabou! Pula para a próxima nota
      thisNote += 2;

      // Verifica se a música acabou
      if (thisNote >= total_elementos) {
        tocando_musica_secreta = false;  // Desliga a flag global
        thisNote = 0;                    // Reseta para a próxima vez
        fase_atual = 0;
        PWM_STOP();
      } else {
        fase_atual = 0;  // Volta para o Estado 0 para processar a nova nota
      }
      uartPutString("tempo pausa\r\n");
    }
  }
}

void Musica_secreta(void) {
  // 1. INICIALIZAÇÃO
  unsigned int tempo = 140;
  wholenote = (60000 * 4) / tempo;
  total_elementos = sizeof(melody) / sizeof(melody[0]);
  // Liga a flag global para a máquina de estados saber que deve rodar
  tocando_musica_secreta = true;

  while (tocando_musica_secreta == true) {
    // Atualiza a música (A FSM vai calcular as notas e tocar)
    // Dica: faça a sua função receber o wholenote como parâmetro!
    Gerenciar_Musica_Assincrona();

    // Atualiza a animação do TM1637 (Passando o tempo da semínima)
    display_animacao_circular(wholenote / 4);

    // Atualiza a animação da torre de LEDs
    Animacao_Leds_Torre(wholenote / 4);
  }

  // 3. ENCERRAMENTO
  PWM_ENABLE(DESLIGAR);
  APAGAR_TODOS_LEDS_COLUNA();  // Adapte para a sua macro de apagar LEDs
}