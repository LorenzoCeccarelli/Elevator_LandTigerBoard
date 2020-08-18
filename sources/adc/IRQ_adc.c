/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../DAC/dac.h"
#include "../TouchPanel/TouchPanel.h"

#define DIV 341

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

extern nota tonoscelto;
extern int n_bottone;
unsigned short AD_current;   
unsigned short AD_last=A;
extern nota struttura_nota[numero_note];

void ADC_IRQHandler(void) {
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
	//mappo il valore letto dal potenziometro su numero_note valori 
	//DIV è stato calcolato dividento FFF (max numero ricevuto da potenziometro) per numero_note
	AD_current= AD_current/DIV;
  if(AD_current!=AD_last){
		tonoscelto=struttura_nota[AD_current];
		Aggiorna_GUI_selezioneTono(n_bottone,tonoscelto);
		AD_last = AD_current;
  }
	
}
