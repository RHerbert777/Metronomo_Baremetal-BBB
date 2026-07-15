#include "interupt.h"
#include "timer.h"
//prcm timer 7
#define CM_PER_TIMER7_CLKCTRL 0x7C
#define CLKSEL_TIMER7_CLK     0x04

volatile uint8_t flag_start = false;
volatile uint8_t flag_SET   = false;
volatile uint8_t flag_UP    = false;
volatile uint8_t flag_DOWN  = false;

volatile uint32_t sistema_ms = 0;//relogio global

//habilita interupção global
void IntAINTCInit(void) {
    HWREG(SOC_AINTC_REGS + INTC_SYSCONFIG) = INTC_SYSCONFIG_SOFTRESET;
    
    while((HWREG(SOC_AINTC_REGS + INTC_SYSSTATUS) & INTC_SYSSTATUS_RESETDONE) != INTC_SYSSTATUS_RESETDONE);
    
    HWREG(SOC_AINTC_REGS + INTC_THRESHOLD) = INTC_THRESHOLD_PRIORITYTHRESHOLD;
}

//Destanca interupção do modulo
void AINTC_Destrancar_Interrupcao(uint32_t modulo_base) {
    if (modulo_base == SOC_DMTIMER_7_REGS) {
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR2) = (1 << 31); // Interrupção 95
    } 
    else if (modulo_base == SOC_DMTIMER_2_REGS) {
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR2) = (1 << 4);  // Interrupção 68
    } 
    else if (modulo_base == SOC_DMTIMER_3_REGS) {
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR2) = (1 << 5);  // Interrupção 69
    } 
    else if (modulo_base == SOC_GPIO_1_REGS) {
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR3) = (1 << 2);  // Interrupção 98
    } 
    else if (modulo_base == SOC_GPIO_2_REGS) {
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR1) = (1 << 0);  // Interrupção 32 (Faltava este!)
    }
}
//Função que seta a interupção para um modulo especifico
//Recebe modulo, pino e o gatilho borda de subida ou descida
void gpioInterruptSetup(uint32_t modulo_gpio, uint8_t pino, uint8_t tipo_gatilho) {
    //habilitando
    HWREG(modulo_gpio + GPIO_IRQSTATUS_SET_0) |= (1 << pino);

    //zerando antigas
    HWREG(modulo_gpio + GPIO_RISINGDETECT)  &= ~(1 << pino);
    HWREG(modulo_gpio + GPIO_FALLINGDETECT) &= ~(1 << pino);

    // Tipo de borda
    if (tipo_gatilho == EDGE_RISING || tipo_gatilho == EDGE_BOTH) {
        HWREG(modulo_gpio + GPIO_RISINGDETECT) |= (1 << pino);
    }
    if (tipo_gatilho == EDGE_FALLING || tipo_gatilho == EDGE_BOTH) {
        HWREG(modulo_gpio + GPIO_FALLINGDETECT) |= (1 << pino);
    }

    // qual o modulo 
    if (modulo_gpio == SOC_GPIO_0_REGS) {
        // GPIO0 é a interrupção 96 -> MIR_CLEAR3 (Bit 0)
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR3) = (1 << 0); 
    }
    else if (modulo_gpio == SOC_GPIO_1_REGS) {
        // GPIO1 é a interrupção 98 -> MIR_CLEAR3 (Bit 2)
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR3) = (1 << 2); 
    }
    else if (modulo_gpio == SOC_GPIO_2_REGS) {
        // GPIO2 é a interrupção 32 -> MIR_CLEAR1 (Bit 0)
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR1) = (1 << 0); 
    }
    else if (modulo_gpio == SOC_GPIO_3_REGS) {
        // GPIO3 é a interrupção 62 -> MIR_CLEAR1 (Bit 30)
        HWREG(SOC_AINTC_REGS + INTC_MIR_CLEAR1) = (1 << 30); 
    }
}

void Timer7_Start_System_Tick(void) {
    
    //  CONFIGURAÇÃO DE ENERGIA E RELÓGIO (PRCM)
    // Liga a energia geral do módulo Timer 7 (Enable = 0x2)
    HWREG(SOC_CM_PER_REGS + CM_PER_TIMER7_CLKCTRL) |= 0x2;

    while ((HWREG(SOC_CM_PER_REGS + CM_PER_TIMER7_CLKCTRL) & 0x30000) != 0);

    // Conecta o Timer 7 ao Cristal de 24 MHz (CLK_M_OSC = 0x1)
    // Isso garante que a matemática de 24.000 tiques por ms funcione perfeitamente!
    HWREG(SOC_CM_DPLL_REGS + CLKSEL_TIMER7_CLK) = 0x1;


    // CONFIGURAÇÃO DO TEMPO E AUTO-RELOAD    
    // O valor de recarga para exatos 1 milissegundo (24.000 ciclos)
    uint32_t countVal = 0xFFFFA240; 

    // Carrega o valor inicial no contador principal
    HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCRR) = countVal;
    
    while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) != 0);

    // Carrega o valor no registrador espelho!
    HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TLDR) = countVal;

    while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) != 0);

    // Habilita a interrupção por Overflow (Estouro) dentro do módulo do Timer
    HWREG(SOC_DMTIMER_7_REGS + DMTIMER_IRQENABLE_SET) = 0x2;

    // Destranca a porta 95 no controlador global AINTC (A função que criamos!)
    AINTC_Destrancar_Interrupcao(SOC_DMTIMER_7_REGS);

    // Dá o Start no Timer COM o bit de Auto-Reload ligado
    HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TCLR) |= 0x3;
    
    while (HWREG(SOC_DMTIMER_7_REGS + DMTIMER_TWPS) != 0);
}

void ISR_Handler(){
    unsigned int intrNum = HWREG(SOC_AINTC_REGS + INTC_SIR_IRQ) & 0x7F;

    if (intrNum == 98) { //if gpio1
        uint32_t status_gpio1 = HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_0);

        // Verifica o NOVO pino do START (Pino 15)
        if (status_gpio1 & (1 << 15)) { 
            flag_start = true;
            HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_0) = (1 << 15); // Abaixa a bandeira
        }
        
        // Verifica o NOVO pino do DOWN (Pino 28)
        if (status_gpio1 & (1 << 28)) { 
            flag_DOWN = true;
            HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_0) = (1 << 28); // Abaixa a bandeira
        }
        
        // O Dummy Read para garantir que a placa não trave!
        HWREG(SOC_GPIO_1_REGS + GPIO_IRQSTATUS_0);
    }else if(intrNum == 32){ //if gpio2
        uint32_t status_gpio2 = HWREG(SOC_GPIO_2_REGS + GPIO_IRQSTATUS_0);

        if (status_gpio2 & (1 << 1)) {
            flag_SET = true;
            
            HWREG(SOC_GPIO_2_REGS + GPIO_IRQSTATUS_0) = (1 << 1);
        }
        if (status_gpio2 & (1 << 4)) {
            flag_UP = true;
            
            HWREG(SOC_GPIO_2_REGS + GPIO_IRQSTATUS_0) = (1 << 4);
        }
        HWREG(SOC_GPIO_2_REGS + GPIO_IRQSTATUS_0);
    }else if(intrNum == 62){ //if gpio 3
        //trabalho final
    }else if (intrNum == 95){ //timer 7
        HWREG(SOC_DMTIMER_7_REGS + 0x28) = 0x2;
        sistema_ms++;

        HWREG(SOC_DMTIMER_7_REGS + 0x28);//esperamos 
    }
    
    //Desligando interupção
    HWREG(SOC_AINTC_REGS + INTC_CONTROL) = 0x1;
}