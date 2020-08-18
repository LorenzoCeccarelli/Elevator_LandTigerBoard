/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        High level joystick management functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "joystick.h"
#include "../led/led.h"

/******************************************************************************
** Function name:	 up_e_attivo
**
** Descriptions:		Funzione che controlla se è stato premuto UP
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
int up_e_attivo(){
	int attivo=0;
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0)
		attivo=1;
	return attivo;
}

/******************************************************************************
** Function name:	 down_e_attivo
**
** Descriptions:		Funzione che controlla se è stato premuto DOWN
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
int down_e_attivo(){
	int attivo=0;
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0)
		attivo=1;
	return attivo;
}
/******************************************************************************
** Function name:	 select_e_attivo
**
** Descriptions:		Funzione che controlla se è stato premuto SELECT
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
int select_e_attivo(){
	int attivo=0;
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0)
		attivo=1;
	return attivo;
}
