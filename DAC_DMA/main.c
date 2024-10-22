/**
 * Se generará una señal triangular con un periodo de 100ms y una amplitud de 3.3V a maxima resolución,
 * esta señal se transmite por DAC.
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <lpc17xx_pinsel.h>
#include <lpc17xx_dac.h>
#include <lpc17xx_gpdma.h>
#include <cr_section_macros.h>

#define FREQ_SIGNAL 10
#define SIZE_SAMPLES 2046
#define PCLK_DAC 25000000

void dac_cfg(); //READY!
void dma_cfg();
void generateSignal(); //READY!


GPDMA_LLI_Type LLI;
uint32_t signal[SIZE_SAMPLES];

int main(){

	while(1){}
	return 0;

}

void generateSignal() {
    for(int i = 0; i < SIZE_SAMPLES; i++) {
        if (i < 1024)
        {
            signal[i] = i << 6;//Se desplaza 6 bits para guardar en [15:6] Value.
        } else {
            signal[i] = (SIZE_SAMPLES - i) << 6;//Se desplaza 6 bits para guardar en [15:6] Value.
        }
    }
}

void dac_cfg(){

	PINSEL_CFG_Type pin_cfg;
	pin_cfg.Portnum = PINSEL_PORT_0;
	pin_cfg.Pinnum = PINSEL_PIN_26;
	pin_cfg.Funcnum = PINSEL_FUNC_2;
	pin_cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
	pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pin_cfg); //CONFIGURO PIN DEL DAC

	//Configuro el dac
	DAC_CONVERTER_CFG_Type dac_cfg;
	dac_cfg.DMA_ENA = SET; //DAC-DMA mode
    dac_cfg.DBLBUF_ENA = RESET; //DOBLE BUFFER, outputs mas suaves
	dac_cfg.CNT_ENA = SET; //Enable Counter
	DAC_Init(LPC_DAC);

	//Configuro el intervalo de actualizacion de la muestra
	DAC_SetBias(LPC_DAC, 0);
	DAC_SetDMATimeOut(LPC_DAC, 1221896383); //(PCLK_DAC*1000000)/(FREQ_SIGNAL*SIZE_SAMPLES)

	//Aplico la configuracion que codeé en dac_cfg
	//(primero se inicia el dac con config. predet. con dac_inic
	DAC_ConfigDAConverterControl (LPC_DAC, &dac_cfg);

}

void dma_cfg(){

	GPDMA_LLI_Type lli_cfg;
	lli_cfg.SrcAddr = (uint32_t) signal;
	lli_cfg.DstAddr = (uint32_t) &LPC_DAC->DACR;
	lli_cfg.NextLLI = (uint32_t) &LLI;
	lli_cfg.Control = SIZE_SAMPLES | //Transfer size
					(0<<12)| //1 transfer to the source DAC>>DMA, DMA leera 1 transferencia
					(0<<15)| //1 transfer to the destiny DMA>>DAC, el DAC recibe 1 transferencia
		            (0b010 << 18)|//Source transfer width (32 bits)
		            (0b010 << 21)|//Destination transfer width(32 bits)
					(1<<26)| //Source increment
					(1<<27)| //Address increment
					(0<<31);

    // Configure DMA channel for memory-to-peripheral transfer
    GPDMA_Channel_CFG_Type dma_cfg;
    dma_cfg.ChannelNum = 0;
    dma_cfg.SrcMemAddr = (uint32_t) signal; // Source: DAC waveform signal
    dma_cfg.DstMemAddr = 0; // No memory destination (peripheral)
    dma_cfg.TransferSize = SIZE_SAMPLES;
    dma_cfg.TransferWidth = 0;
    dma_cfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
    dma_cfg.SrcConn = 0;
    dma_cfg.DstConn = GPDMA_CONN_DAC;
    dma_cfg.DMALLI = (uint32_t) &LLI;

    GPDMA_Init();
    GPDMA_Setup(&dma_cfg);
    GPDMA_ChannelCmd(0, ENABLE);

}




//
