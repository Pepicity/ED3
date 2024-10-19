/*
 * Escribir un programa para que por cada presión de un pulsador, la frecuencia de parpadeo disminuya a la mitad
 * debido a la modificación del registro del Match 0.
 * El pulsador debe producir una interrupción por EINT2 con flanco descendente.
 * Adjuntar el código en C.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <lpc17xx_exti.h>
#include <lpc17xx_timer.h>
#include <lpc17xx_pinsel.h>
#include <lpc17xx_clkpwr.h>

uint32_t MatchVal = 500000;

void pin_config();
void timer_config();

int main(void){

	pin_config();
	timer_config();

	while (1){
	}

	return 0;
}

void pin_config(){

	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Portnum = PINSEL_PORT_2;
	pin_cfg.Pinnum = PINSEL_PIN_12;
	pin_cfg.Funcnum = PINSEL_FUNC_1;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_cfg);

	EXTI_InitTypeDef exti_cfg;
	exti_cfg.EXTI_Line = EXTI_EINT2;
	exti_cfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exti_cfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

	EXTI_Init();
	EXTI_Config(&exti_cfg);
	EXTI_ClearEXTIFlag(EXTI_EINT2);

	NVIC_EnableIRQ(EINT2_IRQn);

}

void timer_config(){

	TIM_TIMERCFG_Type tim_cfg;
	tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tim_cfg.PrescaleValue = 99; //For 100mhz PCLK, Timer resolution = 1us,
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tim_cfg);

	TIM_MATCHCFG_Type match_cfg;
	match_cfg.MatchChannel = 0;
	match_cfg.IntOnMatch = DISABLE;
	match_cfg.ResetOnMatch = ENABLE;
	match_cfg.StopOnMatch = DISABLE;
	match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	match_cfg.MatchValue = MatchVal;
	TIM_ConfigMatch(LPC_TIM0, &match_cfg);

	TIM_Cmd(LPC_TIM0, ENABLE);

}

void EINT2_IRQHandler(){

	if(MatchVal <= 4294500001){ //Valor antes que el match desborde

		MatchVal = MatchVal * 2;
		TIM_UpdateMatchValue(LPC_TIM0, 0, MatchVal);
	}
	else if (MatchVal > 4294500001){

		MatchVal = 500000;
		TIM_UpdateMatchValue(LPC_TIM0, 0, MatchVal);

	}

	EXTI_ClearEXTIFlag(EXTI_EINT2);
}



