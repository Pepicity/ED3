/*
 * Escribir el código que configure el timer0 para cumplir con las especificaciones dadas en la figura adjunta.
 * (Pag 510 Figura 115 del manual de usuario del LPC 1769).
 * Considerar una frecuencia de cclk de 100 Mhz y una división de reloj de periférico de 2.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"

#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_clkpwr.h"

#include <cr_section_macros.h>

void config_tmr();

int main(void){

	config_tmr();


	while(1){}
	return 0;
}


void config_tmr(){

	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_CCLK_DIV_2);

	TIM_TIMERCFG_Type tim_cfg;
	tim_cfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
	tim_cfg.PrescaleValue = 2; //Cuenta 0,1,2
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tim_cfg);

	TIM_MATCHCFG_Type match_cfg;
	match_cfg.MatchChannel = 0;
	match_cfg.IntOnMatch = ENABLE;
	match_cfg.StopOnMatch = DISABLE;
	match_cfg.ResetOnMatch = ENABLE;
	match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	match_cfg.MatchValue = 6;
	TIM_ConfigMatch(LPC_TIM0, &match_cfg);

	TIM_Cmd(LPC_TIM0, ENABLE);

}
