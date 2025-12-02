#ifndef FSM_H_
#define FSM_H_

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

#endif /* FSM_STATES_H_ */
