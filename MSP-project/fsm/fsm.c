#include "fsm.h"
#include "../ultrasound-sensor/sensor.h"
#include "../Servo/sg90.h"
#include "../Display/Display.h"
#include "../iot_wifi/iot_wifi.h"

// Costanti di scansione
#define MIN_ANGLE      0
#define MAX_ANGLE      180
#define ANGLE_STEP     5
#define SCAN_DELAY_MS  5

// Variabili di stato definite in main.c
extern volatile RadarState_t CurrentState;
extern int16_t current_angle;
extern int8_t angle_direction;

// Variabili di notifica del sensore
extern volatile bool capture_done;


void run_state_init(Graphics_Context *ctx) {
    // Lascia vuota o usala solo per reset software
    CurrentState = STATE_MOVE_SERVO;
}

void run_state_move_servo(Graphics_Context *ctx) {
    // 1. Controlla i limiti e inverte la direzione
        if (current_angle >= MAX_ANGLE) {
            angle_direction = -ANGLE_STEP;
            // Pulisce la mappa quando inizia la scansione di ritorno (da 180 a 0)
            clearRadarMap(ctx);
        } else if (current_angle <= MIN_ANGLE) {
            angle_direction = ANGLE_STEP;
            // Pulisce la mappa quando inizia la scansione in avanti (da 0 a 180)
            clearRadarMap(ctx);
        }

        Servo_Set_Angle(current_angle);
        __delay_cycles(3000 * SCAN_DELAY_MS);

        CurrentState = STATE_TRIGGER_SENSOR;
}

void run_state_trigger_sensor() {
    // 1. Invia l'impulso TRIG. L'ISR del Timer è ora in attesa.
    sensor_trigger();

    // 2. Transizione
    CurrentState = STATE_WAIT_ECHO;
}

void run_state_wait_echo() {
    static uint32_t safety_counter = 0;
    safety_counter++;

    if (capture_done) {
        capture_done = false;
        safety_counter = 0;
        CurrentState = STATE_PROCESS_DATA;
    }
    // Timeout aggressivo: se l'oggetto è oltre i 2 metri, passa oltre subito
    else if (safety_counter > 15000) {
        safety_counter = 0;
        t_diff = 0;
        capture_done = true;
        CurrentState = STATE_PROCESS_DATA;
    }
}

void run_state_process_data() {
    // 1. Calcola la distanza (sensor_calculate_distance_cm userà t_diff)
    // capture_done viene resettato qui DOPO aver letto
    uint32_t distance = sensor_calculate_distance_cm();

    // 2. Muovi l'angolo dopo la misura
    current_angle += angle_direction;

    CurrentState = STATE_UPDATE_DISPLAY;
}

// In fsm.c
void run_state_update_display(Graphics_Context *ctx) {
    uint32_t dist = sensor_calculate_distance_cm();

    // Aggiorna sempre la mappa (linea verde)
    drawRadar(ctx, current_angle, dist);

    // Aggiorna il numero in alto solo ogni 5 gradi per non pesare sulla SPI
    if (current_angle % 5 == 0) {
        updateUI(ctx, current_angle, dist);
    }
    UART_IoT_SendData(current_angle, (int)dist);

    CurrentState = STATE_MOVE_SERVO;
}
