# Componenti necessari
* Debugger Hardware
* LandTiger board https://os.mbed.com/users/wim/notebook/landtiger-baseboard/
# Utilizzo
Scaricare Keil µVision IDE http://www2.keil.com/mdk5/uvision/
Sono possibili due modi di utilizzo:
* Emulazione software
* Esecuzione hardware (direttamente sulla scheda)
## Emulazione software
- Clonare il repository e aprire il progetto con Keil µVision
- Cliccare su "Options for Target — Debug" e poi "Use simulator"
- Cliccare su "Run"
- Attraverso il debugger si può interagire con il sistema simulato
## Esecuzione sulla scheda
- Clonare il repository e aprire il progetto con Keil µVision
- Cliccare su "Options for Target — Debug" e poi "Use <debugger>"
- Caricare il codice sorgente sulla scheda attraverso Keil
- Cliccare su "Run"
# Descrizione
  Il sistema simula un ascensore per disabili ed è composto da due componenti principali: 
  * **Controller** che è usato dall'utilizzatore per muovere l'ascensore
  * **Due pannelli di richiesta** posti in due piani diversi
  ## Controller
  E' composto da :
  * **Joystick** usato per manovrare l'ascensore
  * **Led di stato** che fornisce un feedback visivo sullo stato dell'ascensore
  ## Pannello di richiesta
  Ogni pannello di richiesta è composto da :
  * **Bottone di prenotazione**, se premuto permette di prenotare l'ascensore
  * 2 LEDs :
    * **Reserved LED** , si accende quando l'ascensore è occupato o prenotato con successo
    * **Alarm LED*, si accende in caso di allarme
