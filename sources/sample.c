/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: Progetto 2 - Architetture dei sistemi di elaborazione (Prof.Paolo Bernardi)
 * Author:  Lorenzo Ceccarelli s269134
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
            
/* library includes */

#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "info_elevator.h"
#include "DAC/dac.h"
#include "ADC/adc.h"
#include "GLCD/GLCD.h"
#include "GLCD/AsciiLib.h"
#include "TouchPanel/TouchPanel.h"

/* global variables */

extern nota struttura_nota[numero_note]; 
extern nota tono1;
extern nota tono2;

/* funtion prototypes */

void Riempi_Struttura_Nota(void);
void SystemSetup(void);

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemSetup();
		
  while (1) {                           /* Loop forever                       */	
 		__ASM("wfi");
  }

}

/******************************************************************************
** Function name:		SystemSetup
**
** Descriptions:		Gestisce il boot del sistema configurando tutti i dispositivi 
**                  utilizzati
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void SystemSetup(){
	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LED_init();                           /* LED Initialization                 */
  BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
	alimenta_timer(2);
	alimenta_timer(3);
	DAC_init();
	ADC_init();
	Riempi_Struttura_Nota();
	LCD_Initialization();
	TP_Init();
	TouchPanel_Calibrate();
	Disegna_StandByMode();
	LED_Out(0);
	init_RIT(Tempo50msRIT);									/* RIT Initialization 50 msec for joystick   	*/
	enable_RIT();													/* RIT enabled												*/
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);	
return;	
}
/******************************************************************************
** Function name:		Riempi_Struttura_Nota
**
** Descriptions:		Riempe il vettore di struct con le informazioni utili sulle
**                  note
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void Riempi_Struttura_Nota(void){
	struttura_nota[C].nome='C' ;
	struttura_nota[C].frequenza=262;
	
	struttura_nota[d].nome='d' ;
	struttura_nota[d].frequenza=277;
	
	struttura_nota[D].nome='D' ;
	struttura_nota[D].frequenza=294;
	
	struttura_nota[e].nome='e' ;
	struttura_nota[e].frequenza=311;
	
	struttura_nota[E].nome='E' ;
	struttura_nota[E].frequenza=330;
	
	struttura_nota[F].nome='F' ;
	struttura_nota[F].frequenza=349;
	
	struttura_nota[g].nome='g' ;
	struttura_nota[g].frequenza=370;
	
	struttura_nota[G].nome='G' ;
	struttura_nota[G].frequenza=392;
	
	struttura_nota[a].nome='a' ;
	struttura_nota[a].frequenza=415;
	
	struttura_nota[A].nome='A' ;
	struttura_nota[A].frequenza=440;
	
	struttura_nota[b].nome='b' ;
	struttura_nota[b].frequenza=466;
	
	struttura_nota[B].nome='B' ;
	struttura_nota[B].frequenza=494;
	
	struttura_nota[c].nome='c';
	struttura_nota[c].frequenza=523;
	
	tono1=struttura_nota[A];
	tono2=struttura_nota[A];
	
}

