/*
 * Utilizando el modo Capture, escribir un código en C  para que guarde en una variable llamada "shooter",
 * el tiempo (en milisegundos) que le lleva a una persona presionar dos pulsadores con un único dedo.
 * Adjuntar  el archivo .c del código.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <lpc17xx_timer.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_clkpwr.h>

#define PR 0 //Prescaler
#define CCLKDIV 1 //Div del prescaler

void config_pin();
void config_tmr();
void time();

uint32_t shooter;
uint32_t cpt0 = 0;
uint32_t cpt1 = 0;


int main(void) {

	config_pin();
	config_tmr();

    while(1) {

    }
    return 0 ;
}

void config_pin(){

	PINSEL_CFG_Type pin_cfg;

	pin_cfg.Portnum = PINSEL_PORT_0;
	pin_cfg.Pinnum = PINSEL_PIN_4;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
	pin_cfg.Funcnum = PINSEL_FUNC_3;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

	pin_cfg.Pinnum = PINSEL_PIN_5;
    PINSEL_ConfigPin(&pin_cfg);

}

void config_tmr(){

	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER2, CLKPWR_PCLKSEL_CCLK_DIV_1); //DIVCCLK = 1

	TIM_TIMERCFG_Type tmr_cfg;
	tmr_cfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	tmr_cfg.PrescaleValue = PR + 1; //PR 0 default
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &tmr_cfg);

	TIM_CAPTURECFG_Type cpt_cfg;
	cpt_cfg.CaptureChannel = 0;
	cpt_cfg.RisingEdge = ENABLE;
	cpt_cfg.FallingEdge = DISABLE;
	cpt_cfg.IntOnCaption = ENABLE;
	TIM_ConfigCapture(LPC_TIM2, &cpt_cfg);

	cpt_cfg.CaptureChannel = 1; //Uso el segundo canal del capture para el segundo toque del dedo
	TIM_ConfigCapture(LPC_TIM2, &cpt_cfg);

	TIM_Cmd(LPC_TIM2, ENABLE);
	NVIC_EnableIRQ(TIMER2_IRQn);

}

void TIMER2_IRQHandler(){

	if(TIM_GetIntStatus(LPC_TIM2, TIM_CR0_INT)){
		cpt0 = TIM_GetCaptureValue(LPC_TIM2, TIM_COUNTER_INCAP0);
	}
	else if (TIM_GetIntStatus(LPC_TIM2, TIM_CR1_INT)){
		cpt1 = TIM_GetCaptureValue(LPC_TIM2, TIM_COUNTER_INCAP1);
		time();
	}

	TIM_ClearIntPending(LPC_TIM2, TIM_CR0_INT);
	TIM_ClearIntPending(LPC_TIM2, TIM_CR1_INT);
}

void time(){

	uint32_t finalcpt = cpt1 - cpt0;

	shooter = (((finalcpt + 1) * (PR + 1))/(100000000/CCLKDIV))*1000;

}
