#include "fsm.h"
#include "sensor.h"
#include "sg90.h"
#include "edumkii.h"

// Costanti di scansione
#define MIN_ANGLE      0
#define MAX_ANGLE      180
#define ANGLE_STEP     2
#define SCAN_DELAY_MS  50

// Variabili di stato definite in main.c
extern volatile RadarState_t CurrentState;
extern int16_t current_angle;
extern int8_t angle_direction;

// Variabili di notifica del sensore
extern volatile bool capture_done;


void run_state_init() {
    // 1. Inizializzazione Hardware
    EDUMKII_init();
    SG90_init();
    sensor_init();

    // 2. Transizione al primo stato operativo
    CurrentState = STATE_MOVE_SERVO;
}

void run_state_move_servo() {
    // 1. Controlla i limiti e inverte la direzione
    if (current_angle >= MAX_ANGLE) {
        angle_direction = -ANGLE_STEP;
    } else if (current_angle <= MIN_ANGLE) {
        angle_direction = ANGLE_STEP;
    }

    // 2. Imposta l'angolo del servo (SG90_set_angle gestisce la generazione PWM)
    SG90_set_angle(current_angle);

    // 3. Introduci un piccolo ritardo per permettere al servo di stabilizzarsi
    // Questo ritardo deve essere calibrato.
    MAP_SysCtl_delay(48000 * (SCAN_DELAY_MS / 2) / 3);

    // 4. Transizione
    CurrentState = STATE_TRIGGER_SENSOR;
}

void run_state_trigger_sensor() {
    // 1. Invia l'impulso TRIG. L'ISR del Timer è ora in attesa.
    sensor_trigger();

    // 2. Transizione
    CurrentState = STATE_WAIT_ECHO;
}

void run_state_wait_echo() {
    // Questa è una FSM Non-Blocking (o semi-blocking)
    // Il main loop attende qui finché l'ISR del sensore (TA0_N_IRQHandler)
    // imposta 'capture_done' a TRUE.
    if (capture_done) {
        // La misurazione è terminata (Interrupt ricevuto)
        CurrentState = STATE_PROCESS_DATA;
    } else {
        // Non è ancora pronto, resta nello stesso stato (polling sulla flag)
        // Alternativa: Inserire MAP_PCM_gotoLPM0() per il risparmio energetico
    }
}

void run_state_process_data() {
    uint32_t distance_cm;

    // 1. Ottiene la distanza dall'ISR
    distance_cm = sensor_calculate_distance_cm();

    // 2. Aggiorna l'angolo per il prossimo ciclo
    current_angle += angle_direction;

    // (La distanza e l'angolo vengono passati implicitamente o esplicitamente
    // alla funzione di visualizzazione, a seconda di come l'hai definita)

    // 3. Transizione
    CurrentState = STATE_UPDATE_DISPLAY;
}

void run_state_update_display() {
    // 1. Disegna il punto sul display LCD
    // Assumiamo che la distanza calcolata nell'ultimo stato sia accessibile qui.
    uint32_t distance_cm = sensor_calculate_distance_cm(); // Riafferra l'ultima distanza
    EDUMKII_draw_radar(current_angle, distance_cm);

    // 2. Transizione al prossimo ciclo di scansione
    CurrentState = STATE_MOVE_SERVO;
}
