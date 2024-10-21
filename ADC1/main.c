/*
 * @file main.c
 * @brief Timer0 and ADC example
 *
 * A traditional wood-fired oven has been constructed to prepare the region's most flavorful roasted goat (cabritos).
 * Using an analog temperature sensor, the temperature inside the oven is monitored and displayed via three LEDs
 * (Green: Less than 40 degrees, Yellow: Less than 70 degrees, Red: Greater than 70 degrees).
 *
 * This program uses Timer0 to trigger periodic ADC conversions every 60 seconds, reading the temperature value
 * and turning on the corresponding LED based on the temperature range. The temperature sensor
 * is connected to ADC channel 7,
 * and the ADC is configured with a 12-bit resolution,
 * providing 4096 discrete values between 0V and Vref (typically 3.3V).
 * The ADC sampling frequency is set to 100 kHz, and 8 readings are averaged to improve accuracy.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"

#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"


#include <cr_section_macros.h>

#define GREEN_LED  1<<22
#define YELLOW_LED  1<<23
#define RED_LED  1<<24
#define OUTPUT  1
#define ADC_FREQ 100000

static uint32_t adc_value = 0;

void pin_cfg(); //Ready!
void tmr_cfg(); //Ready!
void adc_cfg(); //Ready!
void update_leds();
//Timer handler READY!
//adc handler READY!

int main(){

	pin_cfg();
	tmr_cfg();
	adc_cfg();

	while (1){

	}

	return 0;

}

void pin_cfg(){

	//3 LEDS
	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Portnum = PINSEL_PORT_0;
	pin_cfg.Pinnum = PINSEL_PIN_22;
	pin_cfg.Funcnum = PINSEL_FUNC_0;
	pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_cfg); //GREEN LED

	pin_cfg.Pinnum = PINSEL_PIN_23;
	PINSEL_ConfigPin(&pin_cfg); //YELLOW LED

	pin_cfg.Pinnum = PINSEL_PIN_24;
	PINSEL_ConfigPin(&pin_cfg); //RED LED

	GPIO_SetDir(0, GREEN_LED | YELLOW_LED | RED_LED, OUTPUT);
	GPIO_ClearValue(0, GREEN_LED | YELLOW_LED | RED_LED);
}

void tmr_cfg(){

	TIM_TIMERCFG_Type tmr_cfg;
	tmr_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
	tmr_cfg.PrescaleValue = 1000;
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tmr_cfg);

	TIM_MATCHCFG_Type match_cfg;
	match_cfg.MatchChannel = 0;
	match_cfg.MatchValue = 60000; // 60 segundos = 1000us del prescaler * 60000 valor del match
	match_cfg.IntOnMatch = ENABLE;
	match_cfg.ResetOnMatch = ENABLE;
	match_cfg.StopOnMatch = DISABLE;
	match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	TIM_ConfigMatch(LPC_TIM0, &match_cfg);

	TIM_Cmd(LPC_TIM0, ENABLE);
	NVIC_EnableIRQ(TIMER0_IRQn);

}

void adc_cfg(){

	ADC_Init(LPC_ADC, ADC_FREQ);
	ADC_ChannelCmd (LPC_ADC, 7 , ENABLE);
	ADC_IntConfig (LPC_ADC, ADC_ADINTEN7, SET);

}

void TIMER0_IRQHandler(){

	ADC_StartCmd(LPC_ADC, ADC_START_NOW);
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}

void ADC_IRQHandler(){

	NVIC_DisableIRQ(ADC_IRQn);

	for(uint32_t i = 0; i < 8; i++){
		while(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_7, ADC_DATA_DONE)){
		adc_value += ADC_ChannelGetData(LPC_ADC, 7);
		}
	}

	adc_value /= 8;
	NVIC_EnableIRQ(ADC_IRQn);
	update_leds();

}

void update_leds(){

	if(adc_value < 40){
		GPIO_ClearValue(0, YELLOW_LED | RED_LED);
		GPIO_SetValue(0, GREEN_LED);
	}
	else if(adc_value < 70){
		GPIO_ClearValue(0, GREEN_LED | RED_LED);
		GPIO_SetValue(0, YELLOW_LED);
	}
	else{
		GPIO_ClearValue(0, GREEN_LED | YELLOW_LED);
		GPIO_SetValue(0, RED_LED);
	}

}


