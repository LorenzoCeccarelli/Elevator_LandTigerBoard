#include "info_elevator.h"
static int piano=0;
static int posizione=0; 
	

void setta_piano(){//permette di settare il piano
	if (piano==0)
		piano=1;
	else piano=0;
}
void setta_posizione(int p){ //permette di settare la posizione
	posizione=p;
}
int piano_elevator(){ //ritorna il piano corrente dell'ascensore
	return piano;
}
int posizione_elevator(){ //ritorna la posizione dell'ascensore
	return posizione;
}
void setta_piano2(int f){
	piano=f;
	return;
}
