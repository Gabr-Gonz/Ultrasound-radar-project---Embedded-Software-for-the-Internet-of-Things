#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <ti/grlib/grlib.h>

#include "LcdDriver/Crystalfontz128x128_ST7735.h"

#include <stdio.h>

#include <stdint.h>

#include <stdbool.h>



#include "ultrasound-sensor/sensor.h"

#include "Servo/sg90.h"

#include "Display/Display.h"

#include "fsm/fsm.h"

#include "iot_wifi/iot_wifi.h"



// Variabile globale per il contesto grafico

Graphics_Context g_sContext;



// Stato della FSM

volatile RadarState_t CurrentState = STATE_INIT;



// Variabili di Scansione

int16_t current_angle = 0;

int8_t angle_direction = 2;



   void main(void) {

    // 1. Configurazione Base

    MAP_WDT_A_holdTimer();

   // MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

   // MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);



    // 2. INIZIALIZZAZIONE HARDWARE (Sequenziale e sicura)

    Crystalfontz128x128_Init();

    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);



    // Configura i parametri grafici base

    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);

    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);

    Graphics_clearDisplay(&g_sContext);



    // 3. Inizializza i moduli (Chiamiamo le init direttamente invece di passare per la FSM)

    Servo_Init();

    sensor_init();

    UART_IoT_Init();

    initDisplayUI(&g_sContext); // Disegna l'intestazione fissa

    displayUI(&g_sContext);     // Disegna le etichette fisse



    // 4. ABILITAZIONE INTERRUPT (Solo ora che l'hardware è pronto)

    MAP_Interrupt_enableMaster();



    // 5. SET START STATE (Saltiamo l'init della FSM per evitare loop)

    CurrentState = STATE_MOVE_SERVO;



    while (1) {

        switch (CurrentState) {

            case STATE_MOVE_SERVO:      run_state_move_servo(&g_sContext); break;

            case STATE_TRIGGER_SENSOR:  run_state_trigger_sensor(); break;

            case STATE_WAIT_ECHO:       run_state_wait_echo(); break;

            case STATE_PROCESS_DATA:    run_state_process_data(); break;

            case STATE_UPDATE_DISPLAY:  run_state_update_display(&g_sContext); break;

            default:                    CurrentState = STATE_MOVE_SERVO; break;

        }

    }

}

