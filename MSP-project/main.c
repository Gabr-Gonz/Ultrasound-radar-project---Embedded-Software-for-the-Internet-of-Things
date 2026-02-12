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


Graphics_Context g_sContext;    // global variable for the graphics context

volatile RadarState_t CurrentState;     // variable to keep track of the current state of the finite state machine

int16_t current_angle = 0;      // variable useful for the scanning, initialized at 0 for the beginning of the scan

int8_t angle_direction = 5;        // variable used to acknowledge in which direction the servo has to rotate and how much, initialized at five so it will move 5 degrees each time, starting from the positive range



   void main(void){

    MAP_WDT_A_holdTimer();      // macro used to stop the watchdog timer

    // initialization of the display and its context, hardware bound part of code
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);

    // configuration of the map on the display
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);

    // initialization of the modules used in the whole project
    Servo_Init();
    sensor_init();
    UART_IoT_Init();
    initDisplayUI(&g_sContext);
    displayUI(&g_sContext);

    MAP_Interrupt_enableMaster();       // set the system to make it able to receive interrupts

    CurrentState = STATE_MOVE_SERVO;     // set the first state of the FSM

    while(1){

        switch(CurrentState){

            case STATE_MOVE_SERVO:      run_state_move_servo(&g_sContext); break;

            case STATE_TRIGGER_SENSOR:  run_state_trigger_sensor(); break;

            case STATE_WAIT_ECHO:       run_state_wait_echo(); break;

            case STATE_PROCESS_DATA:    run_state_process_data(); break;

            case STATE_UPDATE_DISPLAY:  run_state_update_display(&g_sContext); break;

            default:                    CurrentState = STATE_MOVE_SERVO; break;

        }

    }

}

