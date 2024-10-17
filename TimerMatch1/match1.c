/*
 * Utilizando el Timer 1, escribir un código en C para que por cada presión de un pulsador,la frecuencia
 * de parpadeo de un led disminuya a la mitad debido a la modificación del reloj que llega al periférico.
 *
 * El pulsador debe producir una interrupción por GPIO0 con flanco descendente.
 * Adjuntar el código en C.
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

#define PULSADOR 1<<26

void config_gpio();
void config_tmr();

int main(void){

	config_gpio();
	config_tmr();


	while(1){}
	return 0;
}

void config_gpio(){


	//Config PIN PULSADOR
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Portnum = PINSEL_PORT_0;
	pin_cfg.Pinnum = PINSEL_PIN_26;
	pin_cfg.Funcnum = PINSEL_FUNC_0;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_cfg);


	//Config PIN LED (Se enciende o se apaga segun interrumpa el match 1 del timer 1)
	pin_cfg.Portnum = PINSEL_PORT_1;
	pin_cfg.Pinnum = PINSEL_PIN_22;
	pin_cfg.Funcnum = PINSEL_FUNC_3;
	PINSEL_ConfigPin(&pin_cfg);

	//Config GPIO
	GPIO_SetDir(0, PULSADOR, 0);
	GPIO_ClearValue(0, PULSADOR);

	//Config INT por GPIO
	GPIO_IntCmd(0, PULSADOR, 1);
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_SetPriority(EINT3_IRQn, 1);

}

void config_tmr(){

	//Configuro el PCLK del Timer1
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_1);


	TIM_TIMERCFG_Type tim_cfg;
	tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tim_cfg.PrescaleValue = 100;
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &tim_cfg);

	TIM_MATCHCFG_Type match_cfg;
	match_cfg.MatchChannel = 0;
	match_cfg.IntOnMatch = ENABLE;
	match_cfg.StopOnMatch = DISABLE;
	match_cfg.ResetOnMatch = ENABLE;
	match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	match_cfg.MatchValue = 100000;
	TIM_ConfigMatch(LPC_TIM1, &match_cfg);

}

void EINT3_IRQHandler(){

	switch(CLKPWR_GetPCLKSEL(CLKPWR_PCLKSEL_TIMER1)){

	case 0x00: //CCLK/4
        LPC_SC->PCLKSEL0 |= (0b11 << 4); // CCLK/8 --> El driver no tiene para configurar el divisor en 8
		break;

	case 0x01: //CCLK/1 (Sin division)
		CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_2);
		break;

	case 0x02: //CCLK/2
		CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_4);
		break;

	case 0x03: //CCLK/8
		CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1, CLKPWR_PCLKSEL_CCLK_DIV_1);
		break;
	}

	GPIO_ClearInt(1, PULSADOR);

}




