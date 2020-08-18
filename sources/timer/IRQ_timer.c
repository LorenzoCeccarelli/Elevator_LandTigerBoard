/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../info_elevator.h"
#include "../RIT/RIT.h"
#include "../DAC/dac.h"
#include "../ADC/ADC.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
/*
uint16_t SinTableMax[45] =                                      
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};*/

/* Vettore della sinusoide campionata usata come input per il DAC per generare il suono*/

uint16_t SinTableMax[45] =                                       //per la sinusoide, valori limitati del 30% rispetto all'ampiezza massima
{
    154, 175, 196, 216, 235, 252, 268, 289, 292,
    300, 305, 307, 306, 303, 296, 286, 274, 260, 
    244, 226, 206, 186, 164, 143, 121, 101, 81,
    63, 47, 33 , 21 , 11 , 4 , 1  , 0  , 2  ,   
    7 , 15 , 26 , 39, 55, 72, 91, 111, 132
};

/* variables definitions */
static	int up_down=1; //serve per il lampeggio

volatile int lampeggi=0; //serve per il lampeggio

static int tono=0; //gestisce lo switch del tono durante l'attività dello speaker

volatile nota tonoscelto; //indica la nota scelta con il potenziometro 

volatile int n_bottone; //indica quale nota l'utente desidera modificare durante la maintenance mode

/*extern variables */
extern int piano; 
extern int alarmleds;
extern int posizione;
extern int oneminute;
extern char verso;
extern int settaReservedLeds;
extern int reservedleds;
extern int lampeggio_per_3_secondi;
extern int alarmtimer;
extern int speaker; 
extern nota tono1;
extern nota tono2;
extern int maintenance_mode;
static int modifica_tono1=0;
static int modifica_tono2=0;


/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
										Scatena interrupt ogni 50 ms
										Usato per emulare lo spostamento dell'ascensore durante la fase di prenotazione.
										Viene attivato se l'ascensore si trova al piano opposto rispetto all'utente
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	//Timer0 scatena interrupt ogni 50 ms
	Spostamento(verso); //aggiorna la variabile posizione
	
	//verifico se sono arrivato
	if ((posizione>=distanzap1p2 && verso=='+')|| (posizione<=0 && verso=='-')){ 
		//se si devo lampeggiare lo status led a 5Hz e attivare il timer3 finche non premo il select
		reset_timer(1);
		init_timer(1,TempoLampeggio5Hz);
		enable_timer(1);
		reset_timer(0);
		disable_timer(0);
		lampeggio_per_3_secondi=1;
		lampeggi=0;
		GestionePrenotato_NoSelect();
		if (verso=='+')
			piano=1;
		else piano=0;
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
										Gestisce il lampeggio dei led
										Ha una doppia funzione:
										1-Lampeggio a tempo indeterminato
										2-Lampeggio per 3 secondi
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	/* funzionalità 1 */
	//se sono passati 3 secondi e il sistema ha imposto di spegnere i leds dopo 3 secondi (condizione di terminazione) 
	if (lampeggi==num_lampeggi_in_3_s && lampeggio_per_3_secondi==1){ 
		lampeggio_per_3_secondi=0;
		disable_timer(1);
		LED_Off(LED4);
		lampeggi=0;
		//se il sistema ha imposto di settare i leds di prenotazione
		if (settaReservedLeds==1){
			LED_Off(LED9);
			LED_Off(LED11);
			settaReservedLeds=0;
			reservedleds=0;
		}
		LPC_TIM1->IR =	1;
		return;
	}
	/*funzionalità 2 */
	// lampeggio
	if (up_down==0){
		LED_Off(LED4);
		up_down++;
	}else {
		LED_On(LED4);
		up_down--;
	}
	//aggiorno 
	if (tono==0)
		tono=1;
	else tono=0;
	
	lampeggi++;
  LPC_TIM1->IR =	1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler 
										Ha una doppia funzionalità:
										-Timer di allarme 
										-Timer che gestisce l'attività dello speaker
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{ 
	static int ticks=0;
	int val;
	/*funzionalità 1 */
	//timer 2 ha doppia funzionalità (timer di allarme e timer di speaker)
	if (alarmtimer==1){
	//se scatta timer2 e accendo led 8 e 9 (alarm leds); entro in emergency mode
	Entra_in_EmergencyMode();
	alarmtimer=0;
	reset_timer(2);
	disable_timer(2);
	}
	/* funzionalità 2 */
	if (speaker==1){
		//gestisco lo speaker e quindi il DAC
		DAC_convert(SinTableMax[ticks]<<6);
		ticks++;
		//se il periodo dell'onda è terminato, resetto
		if (ticks==45)
			ticks=0;
		//suono il tono2
		if (tono==1){
			reset_timer(2);
			val=25000000/(45*tono2.frequenza);
			init_timer(2,val);
			}
		//suono il tono1
		if (tono==0){
			reset_timer(2);
			val=25000000/(45*tono1.frequenza);
			init_timer(2,val);
			}
		
		enable_timer(2);
	}
  LPC_TIM2->IR =	1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
** Function name:		Timer3_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
										Ha una doppia funzionalità 
										1-Gestisce il caso se l'ascensore è prenotato ma entro 1 minuto SELECT non viene premuto
										2-Gestisce la modalità manutenzione
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{ //timer 3 ha una doppia funzionalità
	/* funzionalità 1*/
	if (oneminute==1){
	//se scatta timer3 dopo un minuto il select non è stato premuto
	LED_Off(LED9);
	LED_Off(LED11);
	LED_Off(LED4);
	reservedleds=0;
	oneminute=0;
	}
	/* funzionalità 2*/
	//controllo se sono in modalità manutenzione
	if (maintenance_mode==1){
		//catturo il punto premuto. Out of bound se il touch display non è premuto
		getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
		//se il touch display è premuto
		if (display.x<220 && display.x >10 && display.y >35 && display.y<300){
		//controllo se ho premuto il bottone per la configurazione del tono 1
		if (display.x <220 && display.x >10 && display.y>35 && display.y <160 ){
			n_bottone=1;
			tonoscelto=tono1;
			Aggiorna_GUI_pressioneBottone(n_bottone);
			modifica_tono2=0;
			modifica_tono1=1;
		}
		//controllo se ho premuto il bottone per la configurazione del tono 2
		if (display.x <220 && display.x >10 && display.y>160 && display.y <240){
			n_bottone=2;
			tonoscelto=tono2;
			Aggiorna_GUI_pressioneBottone(n_bottone);
			modifica_tono1=0;
			modifica_tono2=1;
		}
	}
		//se l'utente deve modificare il tono 1 devo abilitare il potenziometro
		if (modifica_tono1==1 && modifica_tono2==0){
			ADC_start_conversion();
		}
		//se l'utente deve modificare il tono 2 devo abilitare il potenziometro
		if (modifica_tono2==1 && modifica_tono1==0){
			ADC_start_conversion();
			
		}
		//se clicco su salva metto i toni scelti come toni di default
		if (display.x>10 && display.x <100 && display.y >260 && display.y <310){
			//se l'utente ha modificato il tono 1 devo aggiornare
			if (modifica_tono1==1 && modifica_tono2==0){
				tono1=tonoscelto;
				modifica_tono1=0;
				modifica_tono2=0;
		}
			//se l'utente ha modificato il tono 2 devo aggiornare
			if (modifica_tono2==1 && modifica_tono1==0){
				tono2=tonoscelto;
				modifica_tono1=0;
				modifica_tono2=0;
		}
			//aggiorno la GUI
			Disegna_GUI_configuratore();
		}
		//se clicco su exit esco dalla modalità manutenzione
		if (display.x>180 && display.x <230 && display.y >260 && display.y <310){
			Disegna_StandByMode();
			maintenance_mode=0;
			//riattivo il RIT per riottenre tutte le funzionalità
			init_RIT(Tempo50msRIT);
			enable_RIT();
			disable_timer(3);
			//spengo il led accesi con la gestione del touch panel
			LED_Out(0);
			LPC_TIM3->IR = 1;			//clear interrupt flag
			return;
		}
		//mando in polling 
		init_timer(3,Tempo50ms);
		enable_timer(3);
	}
	
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
