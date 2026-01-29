#ifndef FSM_H_
#define FSM_H_
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h> // Risolve l'errore undefined Graphics_Context
#include "../LcdDriver/Crystalfontz128x128_ST7735.h"
// Definizione degli stati della Finite State Machine
typedef enum {
    STATE_INIT = 0,             // Inizializzazione (Configurazione hardware iniziale)
    STATE_MOVE_SERVO,           // Muove il servo all'angolo successivo
    STATE_TRIGGER_SENSOR,       // Invia il segnale TRIG all'ultrasuono
    STATE_WAIT_ECHO,            // Attende l'interrupt di cattura dell'impulso ECHO
    STATE_PROCESS_DATA,         // Calcola la distanza e aggiorna l'angolo
    STATE_UPDATE_DISPLAY        // Disegna il punto rilevato sul display
} RadarState_t;

// Variabile globale per lo stato corrente
extern volatile RadarState_t CurrentState;

void run_state_init(Graphics_Context *ctx);
void run_state_move_servo(Graphics_Context *ctx);
void run_state_trigger_sensor();
void run_state_wait_echo();
void run_state_process_data();
void run_state_update_display(Graphics_Context *ctx);

#endif /* FSM_STATES_H_ */
