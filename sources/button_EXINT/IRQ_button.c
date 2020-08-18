#include "button.h"
#include "lpc17xx.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../info_elevator.h"
#include "../RIT/RIT.h"
extern int down1;
extern int down2;
extern int down0;
/******************************************************************************
** Function name:		EINT0_IRQHandler
**
** Descriptions:		Gestisce l'interrupt di INT0
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	down0=1;
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}

/******************************************************************************
** Function name:		EINT1_IRQHandler
**
** Descriptions:		Gestisce l'interrupt di KEY1
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	down1=1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

/******************************************************************************
** Function name:		EINT2_IRQHandler
**
** Descriptions:		Gestisce l'interrupt di KEY2
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4   &= ~(1 << 24);     /* GPIO pin selection */
	down2=1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


