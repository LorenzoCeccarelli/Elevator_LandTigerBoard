#ifndef _DAC_H
#define _DAC_H
#include <string.h>

//definisco le note
#define C 0
#define d 1
#define D 2
#define e 3
#define E 4
#define F 5
#define g 6
#define G 7 
#define a 8
#define A 9
#define b 10
#define B 11
#define c 12
#define numero_note 13
 
// nota struct defintion
typedef struct {
	char nome;
	int frequenza;
} nota;

/* lib_adc.c */
void DAC_init (void);
void DAC_convert (int value);

#endif
