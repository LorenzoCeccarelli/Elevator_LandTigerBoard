/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../joystick/joystick.h"
#include "../info_elevator.h"
#include "../timer/timer.h"
#include "../DAC/dac.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"

/* variables definitions */

volatile nota struttura_nota[numero_note]; // è un vettore di struct che contiene le informazioni sulle note quali il nome e la frequenza

volatile int down1=0; //variabile di stato per il debouncing di key1

volatile int down2=0; //variabile di stato per il debouncing di key2

volatile int down0=0; //variabile di stato per il debouncing di int0

static int select=0; //select=0 -> select non premuto ; select=1 -> select premuto

static int up=0; //up=0 -> up non premuto ; up=1 -> up premuto

static int downj=0; //downj=0 -> down non premuto ; downj=1 -> down premuto

volatile int posizione=0; //fornisce un valore di posizione (posizione_reale=posizione*spazio_percorso_in_50ms)

static int up_down=0; //serve per il lampeggio a 2Hz mentre l'ascensore si muove

static int conteggio=0; //serve per il lampeggio a 2Hz mentre l'ascensore si muove

volatile int alarmtimer=0; //alarmtimer=1 -> alarm timer attivo (Timer2) else disattivato

volatile int alarmleds=0; //indica se i led di allarme sono accesi

volatile int piano=0; //indica il piano dove è posizionato l'ascensore

volatile int oneminute=0; //indica se l'ascensore è stato prenotato ma ancora l'utente non è salito

volatile char verso; //indica se l'ascensore deve muoversi verso l'alto o verso il basso

volatile int settaReservedLeds=0; //indica se quando viene scatenato l'interrupt del timer questo deve spegnere anche i led di prenotazione

volatile int reservedleds=0; //indica se i leds di prenotazione sono accesi oppure no

volatile int emergency_mode=0; //on off emergency mode

volatile int lampeggio_per_3_secondi; //se uguale a 1 timer 1 lampeggia per 3 secondi. Sennò lampeggia all'infinito

volatile int speaker=0; //se speaker=1 lo speaker è acceso se no è spento

volatile nota tono1; //indica il tono1 di default 

volatile nota tono2; //indica il tono2 di default

static int nhold=0; //serve a capire se int0 è premuto per meno di 2 secondi

volatile int maintenance_mode=0; //indica se il sistema è in modalità manutenzione oppure no

extern int lampeggi;

extern nota tonoscelto;

/* functions definitions */

void Reset_Timer3(void); 

void Controllo_e_setto_TimerAllarme(void);

void Controllo_e_setto_LedsAllarme(void);

void Arrivato_a_Destinazione(int);

void Esci_da_EmergencyMode(void);

void AttivaSpeaker(void);

void DisattivaSpeaker(void);


/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler (void)
{					
	static int emergency=0;
	/*-------------------------------
					GESTIONE JOYSTICK 
	 --------------------------------*/
	
	//joystick funziona solo se l'ascensore è prenotato
	
	/* ------Gestione	SELECT---------*/
	//se non sono ne in modalità emergenza e ne in modalità manutenzione e se ho premuto select è l'ascensore è prenotato
	if(select_e_attivo() && reservedleds==1 && emergency_mode==0 && maintenance_mode==0){
		select=1;
		
		//resetta il timer3 che scatena l'interupt quando l'ascensore è stato prenotato ma dopo 1 minuto non è stato premuto il select
		Reset_Timer3();
		}
	/* ------Gestione UP-------------*/
		//se ho premuto up e l'ascensore è prenotato e non sto in modalità emergenza
	if(up_e_attivo() && reservedleds==1 && emergency_mode==0){
		if (select==1){ //ok posso muovermi
			
			//controllo se è accesso il timer di allarme ovvero se sono fermo durante il transito. Se si lo disattivo
			Controllo_e_setto_TimerAllarme();
			
			//controllo se sono arrivato
			if (posizione>=distanzap1p2) //se si
				Arrivato_a_Destinazione(1);
			
			//se premo up per la prima volta
			if (up==0)
				up=1;
			
			//se sono tra il piano 1 e il piano terra ovvero in transito devo muovermi
			if (up==1)
				Spostamento('+');
		}
	}
	
	/* -------Gestion DOWN----------*/
	//se ho premuto down e l'ascensore è prenotato e non sto in modalità emergenza
	if(down_e_attivo() && reservedleds==1 && emergency_mode==0 ){
		if (select==1){//ok posso muovermi
			
			//controllo se è acceso il timer di allarme ovvero se sono fermo durante il transito. Se si lo disattivo
			Controllo_e_setto_TimerAllarme();
			
			//controllo se sono arrivato
			if (posizione<=0) //se si
				Arrivato_a_Destinazione(0);
	
			//se premo down per la prima volta
			if (downj==0)
				downj=1;
			
			//se sono tra il piano 1 e il piano terra ovvero in transito devo muovermi
			if (downj==1)
				Spostamento('-');
			}
		}
	
	/* -----Gestione utente fermo durante il trnasito */
		//se l'ascensore è attivo e non è premuto ne up e ne down
	if (select==1 && !up_e_attivo() && !down_e_attivo() && reservedleds==1 && emergency_mode==0){ //utente è fermo durante il transito
		//zona che gestisce timer per allarme led
		alarmtimer=1;
		init_timer(2,Tempo1m);
		enable_timer(2);
		//fine zona di allarme
		conteggio=0;
		LED_On(LED4);
		
	}		

	/*----- Button management -------*/
	//KEY1
	if(down1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0) {	/* KEY1 button pressed */
			down1++;		
			switch(down1){
				case 2:
					//verifico se l'ascensore non è in manutenzione
					if (maintenance_mode==0){
						//verifico se sono in emergency mode
						if (emergency_mode==1){
							Esci_da_EmergencyMode();
							verso='+';
							init_timer(0,Tempo50ms);
							enable_timer(0);
							select=0;
					}
						//verifico se l'ascensore è prenotato
						if (reservedleds==0){ //se no lo prenoto
							LED_On(LED9);//led 9 on  (reserved led floor 1)
							LED_On(LED11);//led 11 on (reserved led floor 0)
							reservedleds=1;
							if (piano==0){ //se l'ascensore si trova al piano opposto
								reset_timer(0);
								init_timer(0,Tempo50ms);
								enable_timer(0);
								verso='+';
							}
							else  //se l'ascensore si trova nello stesso piano ho un minuto di tempo per premere il select
								GestionePrenotato_NoSelect();
						}
					}
					break;
				default:
					break;
			}
		}
			else { //button released
				down1=0;
				NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
			}
			
		}
	//KEY2
	if(down2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0) {	/* KEY2 button pressed */
			down2++;			
			switch(down2){
				case 2:
					//verifico se l'ascensore non è in manutenzione
					if (maintenance_mode==0){
						//verifico se sono in emergency mode
						if (emergency_mode==1){
							Esci_da_EmergencyMode();
							verso='-';
							init_timer(0,Tempo50ms);
							enable_timer(0);
							select=0;
					}
						//verifico se l'ascensore è prenotato
						if (reservedleds==0){
							LED_On(LED9);//led 9 on  (reserved led floor 1)
							LED_On(LED11);//led 11 on (reserved led floor 0)
							reservedleds=1;
							if (piano==1){ //se l'ascensore si trova al piano opposto
								reset_timer(0);
								init_timer(0,Tempo50ms);
								enable_timer(0);
								verso='-';
							}
							else //se l'ascensore si trova nello stesso piano ho un minuto di tempo per premere il select
								GestionePrenotato_NoSelect();
					}	
				}
					break;
				default:
					break;
			}
		}			else {
					down2=0;
					NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
					LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	//INT0
	if(down0!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0 && maintenance_mode==0) {	/* INT0 button pressed */
			down0++;	
			//se ho premuto int0 per più di 2 secondi attivo la modalità emergenza
			if (down0>=contatore2s){
				emergency=1;
				Entra_in_EmergencyMode();
				NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
			}
		}			
		else { //button released
			//se ho rilasciato prima di 2 secondi
				if (emergency==0){
					nhold=1;
					}
				down0=0;
				emergency=0;
				NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
				LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	//se ho rilasciato prima di 2 secondi
	if (nhold==1){
		nhold=0;
		//se era attiva l'emergency mode la disattivo
		if (emergency_mode==1){
			Esci_da_EmergencyMode();
			//se l'ascensore si trovava in transito riattivo il joystick
			if (posizione!=0)
				select=1;
		}
		
	}
	
	/*----Gestione touch panel maintenance mode-----*/
	//posso entrare in modalità manutenzione solo se l'ascensore è libero
	if (reservedleds==0){
			//catturo il punto premuto. Se non è premuto ricevo valori out of bound
		getDisplayPoint(&display,Read_Ads7846(),&matrix);
		//verifico se ho premuto sul bottone 
		if (display.x>108 && display.x<132 && display.y<172 && display.y>148){
			//se si entro in modalità manutenzione
			maintenance_mode=1;
			LED_Out(0);
			tonoscelto=tono1;
			//esco dalla modalità stand by ed intro in modalità configurazione
			Disegna_GUI_configuratore();
			//attivo il timer che si occupa della fase di manutenzione
			init_timer(3,Tempo50ms);
			enable_timer(3);
			//disabilito il rit durante la fase di manutenzione
			disable_RIT();
		}
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
	}
/******************************************************************************
** Function name:		Reset_Timer3
**
** Descriptions:		Resetta il timer 3 se l'ascensore si era fermato durante il transito
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void Reset_Timer3(){
		if (oneminute==1){
			oneminute=0;
			reset_timer(3);
			disable_timer(3);
	}
}
/******************************************************************************
** Function name:		Controllo_e_setto_TimerAllarme
**
** Descriptions:		Resetto e disabilito il timer 2 se era acceso
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void Controllo_e_setto_TimerAllarme(){
	if (alarmtimer==1){ //controllo se è acceso il timer di allarme
				reset_timer(2);
				disable_timer(2);
				alarmtimer=0;
			}
}
/******************************************************************************
** Function name:		Controllo_e_setto_LedsAllarme
**
** Descriptions:		Spengo i led di allarme se accesi
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void Controllo_e_setto_LedsAllarme(){
	if (alarmleds==1){
				alarmleds=0;
				LED_Off(LED8);
				LED_Off(LED10);
			}
}
/******************************************************************************
** Function name:		Arrivato_a_Destinazione
**
** Descriptions:		Funzione che verifica se l'ascensore è arrivata a destinazione
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void Arrivato_a_Destinazione(int floor){
	//quando arrivo a destinazione lo status led lampeggia a 5Hz
	lampeggio_per_3_secondi=1;
	lampeggi=0;
	init_timer(1,TempoLampeggio5Hz);
	enable_timer(1);
	//ascensore diventa libero dopo il blinking a 5Hz
	settaReservedLeds=1;
	select=0;
	if (floor==1){
		piano=1;
		posizione=distanzap1p2;
		up=0;
	}else {
		piano=0;
		posizione=0;
		downj=0;
	}
}
/******************************************************************************
** Function name:		Spostamento
**
** Descriptions:		Funzione che gestisce lo spostamento dell'ascensore
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void Spostamento(char v){
	//mentre mi muovo lo status led deve lampeggiare a 2Hz
			if (conteggio==Numero_Lampeggi_a_2Hz){
				if (up_down==0){
					LED_On(LED4);
					up_down++;
				}
				else if (up_down==1){
					LED_Off(LED4);
					up_down--;
				}
				conteggio=0;
			} else conteggio++;
	//controllo il verso dello spostamento
	if (v=='+')
			posizione++;
	else if (v=='-')
			posizione--;
}
/******************************************************************************
** Function name:		GestionePrenotato_NoSelect
**
** Descriptions:		Funzione che gestisce il caso se l'ascensore è prenotato ma non è stato premuto select
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void GestionePrenotato_NoSelect(void){
	//se ho prenotato ma non premo select devo attivare il timer3
	init_timer(3,Tempo1m);
	enable_timer(3);
	oneminute=1;
}
/******************************************************************************
** Function name:		Entra_in_EmergencyMode
**
** Descriptions:		Funzione che abilita l'emergency mode
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void Entra_in_EmergencyMode(void){
	//entro in modalità emergenza
	emergency_mode=1;
	//attivo il lampeggio dello status led
	disable_timer(2);
	disable_timer(3);
	disable_timer(0);
	init_timer(1,TempoLampeggio4Hz);
	enable_timer(1);
	//accendo gli alarmleds
	alarmleds=1;
	LED_On(LED8);
	LED_On(LED10);
	//accendo lo speaker
	AttivaSpeaker();
}
/******************************************************************************
** Function name:		Esci_da_EmergencyMOde
**
** Descriptions:		Funzione che disabilità l'emergency mode
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void Esci_da_EmergencyMode(void){
	//esci dalla modalità emergenza
	emergency_mode=0;
	//disattivo il lampeggio dello status led
	disable_timer(1);
	//spengo gli alarmleds
	alarmleds=0;
	LED_Off(LED8);
	LED_Off(LED10);
	LED_Off(LED4);
	//disattivo lo speaker
	DisattivaSpeaker();
	
}
/******************************************************************************
** Function name:		AttivaSpeaker
**
** Descriptions:		Funzione che attiva lo speaker
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void AttivaSpeaker(void){
	speaker=1;
	//attivo timer 2
	reset_timer(2);
	init_timer(2,tono1.frequenza);
	enable_timer(2);
}

/******************************************************************************
** Function name:		DisattivaSpeaker
**
** Descriptions:		Funzione che disabilita lo speaker
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void DisattivaSpeaker(void){
	speaker=0;
	reset_timer(2);
	disable_timer(2);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
