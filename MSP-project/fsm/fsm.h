#ifndef FSM_H_
#define FSM_H_
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "../LcdDriver/Crystalfontz128x128_ST7735.h"

// definition of the states of the finite state machine
typedef enum {
    STATE_MOVE_SERVO = 0,       // move the servo to the next angle
    STATE_TRIGGER_SENSOR,       // trigger the sensor to capture the value of the current scan
    STATE_WAIT_ECHO,            // wait for the sensor to capture the value
    STATE_PROCESS_DATA,         // calculate the distance captured and update the scan to the next angle
    STATE_UPDATE_DISPLAY        // draw the map on the display with the results of the scan
} RadarState_t;

// variables defined in the main
extern volatile RadarState_t CurrentState;
extern int16_t current_angle;
extern int8_t angle_direction;

void run_state_move_servo(Graphics_Context *ctx);
void run_state_trigger_sensor();
void run_state_wait_echo();
void run_state_process_data();
void run_state_update_display(Graphics_Context *ctx);

#endif /* FSM_STATES_H_ */
