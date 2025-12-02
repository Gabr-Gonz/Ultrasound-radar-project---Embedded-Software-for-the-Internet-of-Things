#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

// I tuoi header dei componenti
#include "sensor.h"
#include "sg90.h"
#include "edumkii.h"
#include "fsm_states.h"

// Variabile di stato globale
volatile RadarState_t CurrentState = STATE_INIT;

// Variabili di Scansione
int16_t current_angle = MIN_ANGLE;
int8_t angle_direction = ANGLE_STEP;

// Variabile di notifica dall'ISR del sensore
extern volatile bool capture_done;

// -----------------------------------------------------------------
// FUNZIONI DI STATO (Da implementare in un file separato o qui)
// -----------------------------------------------------------------

void run_state_init();
void run_state_move_servo();
void run_state_trigger_sensor();
void run_state_wait_echo();
void run_state_process_data();
void run_state_update_display();


void main(void) {
    // ... (Configurazione WDT e Clock come prima) ...
    MAP_WDT_A_holdTimer();
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    MAP_CS_initClockSource(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Abilita gli interrupt globali
    MAP_Interrupt_enableMaster();

    // Loop principale della Finite State Machine
    while (1) {
        switch (CurrentState) {
            case STATE_INIT:
                run_state_init();
                break;
            case STATE_MOVE_SERVO:
                run_state_move_servo();
                break;
            case STATE_TRIGGER_SENSOR:
                run_state_trigger_sensor();
                break;
            case STATE_WAIT_ECHO:
                run_state_wait_echo();
                break;
            case STATE_PROCESS_DATA:
                run_state_process_data();
                break;
            case STATE_UPDATE_DISPLAY:
                run_state_update_display();
                break;
            default:
                // Stato di errore o ripristino
                CurrentState = STATE_INIT;
                break;
        }
        // Opzionale: Inserire una breve pausa o modalità a basso consumo qui
    }
}
// ... (Fine main) ...
