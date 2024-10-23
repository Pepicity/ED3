/**
 * Se toma una señal por el ADC de 1000 muestras y luergo se reproduce por el DAC
 * IMPORTANTE: La frecuencia del ADC esta en modo burst de 200kHz por muestra.
 * La frecuencia de salida del DAC no es la misma que la frecuencia establecida por el TimeOut.
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <lpc17xx_pinsel.h>
#include <lpc17xx_adc.h>
#include <lpc17xx_dac.h>
#include <lpc17xx_gpdma.h>
#include <cr_section_macros.h>

#define FREQ_SIGNAL 1000
#define SIZE_SAMPLES 1000

uint32_t bufferADC = 0;

void config_pin(); // READY!
void config_adc(); //READY!
void config_dac(); //READY!
void config_dma(); // READY!
//TODO Manejo de captura

int main(){

	config_pin();
	config_adc();
	config_dac();
	config_dma();

	while(1){}
	return 0;

}


void config_pin(){

	PINSEL_CFG_Type pin_cfg; //ADC
	pin_cfg.Portnum = PINSEL_PORT_0;
	pin_cfg.Pinnum = PINSEL_PIN_7;
	pin_cfg.Funcnum = PINSEL_FUNC_2;
	pin_cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_cfg);

	pin_cfg.Pinnum = PINSEL_PIN_26; 	//DAC
	PINSEL_ConfigPin(&pin_cfg);



}

void config_adc(){

	ADC_Init(LPC_ADC, 200000);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	ADC_ChannelCmd (LPC_ADC, ADC_CHANNEL_7, ENABLE);

}

void config_dac(){

	uint32_t timeout = 25000000/(1000*1000);

	DAC_CONVERTER_CFG_Type dac_cfg;
	dac_cfg.CNT_ENA = SET;
	dac_cfg.DMA_ENA = SET;
	dac_cfg.DBLBUF_ENA = RESET;

	DAC_Init(LPC_DAC);
	DAC_ConfigDAConverterControl (LPC_DAC, &dac_cfg);
	DAC_SetDMATimeOut(LPC_DAC, timeout);

}

void config_dma(){

	GPDMA_Init();

	//CHANNEL ADC P2M
	GPDMA_Channel_CFG_Type dmaadc_cfg;
	dmaadc_cfg.ChannelNum = 0;
	dmaadc_cfg.TransferSize = SIZE_SAMPLES;
	dmaadc_cfg.SrcMemAddr  = 0;
	dmaadc_cfg.DstMemAddr = (uint32_t) bufferADC;
	dmaadc_cfg.TransferWidth = 0;
	dmaadc_cfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
	dmaadc_cfg.SrcConn = GPDMA_CONN_ADC;
	dmaadc_cfg.DstConn = 0;
	dmaadc_cfg.DMALLI = 0;
	GPDMA_Setup(&dmaadc_cfg);
	GPDMA_ChannelCmd(0, ENABLE);

	//CHANNEL DAC M2P

	GPDMA_LLI_Type lli_cfg;
	lli_cfg.SrcAddr = (uint32_t) bufferADC;
	lli_cfg.DstAddr = (uint32_t) &LPC_DAC->DACR;
	lli_cfg.NextLLI = (uint32_t) &lli_cfg;
	lli_cfg.Control = SIZE_SAMPLES | //Tamaño de la transferencia
					(0<<12) |
					(0<<15) |
					(0b010 <<18)|
					(0b010 <<21)|
					(1<<26) | //Incremento del buffer del ADC
					(0<<27) | //No incremento del DAC (Solo hay uno)
					(0<<31); //Interrupcion desactivada
	//M2P
	dmaadc_cfg.ChannelNum = 1;
	dmaadc_cfg.TransferSize = SIZE_SAMPLES;
	dmaadc_cfg.SrcMemAddr  = (uint32_t) bufferADC;
	dmaadc_cfg.DstMemAddr = 0;
	dmaadc_cfg.TransferWidth = 0;
	dmaadc_cfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	dmaadc_cfg.SrcConn = 0;
	dmaadc_cfg.DstConn = GPDMA_CONN_DAC;
	dmaadc_cfg.DMALLI = (uint32_t) &lli_cfg;
	GPDMA_Setup(&dmaadc_cfg);
	GPDMA_ChannelCmd(1, ENABLE);

}
//
