/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           RIT.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_RIT, funct_RIT, IRQ_RIT .c files
** Correlated files:    lib_RIT.c, funct_RIT.c, IRQ_RIT.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __RIT_H
#define __RIT_H

#define distanzap1p2 144 //144*50ms=7,2 s ; il rit deve scatdenarsi 144 volte per cambiare piano
#define Numero_Lampeggi_a_2Hz 10
#define num_lampeggi_in_3_s 30 
#define contatore2s 40 //    2/0.05=40

/*functions definetions*/
void Spostamento(char);
void GestionePrenotato_NoSelect(void);
void Entra_in_EmergencyMode(void);

/* init_RIT.c */
extern uint32_t init_RIT( uint32_t RITInterval );
extern void enable_RIT( void );
extern void disable_RIT( void );
extern void reset_RIT( void );
/* IRQ_RIT.c */
extern void RIT_IRQHandler (void);

#endif /* end __RIT_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
