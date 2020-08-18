/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        Prototypes of functions included in the lib_timer, funct_timer, IRQ_timer .c files
** Correlated files:    lib_timer.c, funct_timer.c, IRQ_timer.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H
//#define SIMULATORE
#ifdef SIMULATORE
//sono in modalità simulazione. Tempi scalati di un fattore 10!
#define TempoLampeggio5Hz 0x0003D090
#define Tempo3s  					0x007270E0
#define TempoPercorrenza  0x0112A880
#define Tempo50msRIT   	  0x0007A120
#define Tempo1m 	        0x00E4E1C0 //questo è scalato di un fattore 100
#define Tempo50ms         0x0001E848
#define TempoLampeggio4Hz 0x0004C4B4

#else 
#define TempoLampeggio5Hz 0x002625A0					
#define Tempo3s  			    0x047868C0							
#define TempoPercorrenza  0x0ABA9500		
#define Tempo50msRIT   	  0x004C4B40						
#define Tempo1m 	        0x59682F00									
#define Tempo50ms         0x001312D0             
#define TempoLampeggio4Hz 0x002FAF08
#endif

/* init_timer.c */
extern void alimenta_timer(uint8_t timer_num); 
extern uint32_t init_timer( uint8_t timer_num, uint32_t timerInterval );
extern void enable_timer( uint8_t timer_num );
extern void disable_timer( uint8_t timer_num );
extern void reset_timer( uint8_t timer_num );
/* IRQ_timer.c */
extern void TIMER0_IRQHandler (void);
extern void TIMER1_IRQHandler (void);

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
