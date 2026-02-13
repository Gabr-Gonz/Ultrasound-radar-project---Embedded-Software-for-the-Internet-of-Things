#include "fsm.h"
#include "../ultrasound-sensor/sensor.h"
#include "../Servo/sg90.h"
#include "../Display/Display.h"
#include "../iot_wifi/iot_wifi.h"

// constants used for scanning

#define MIN_ANGLE      0
#define MAX_ANGLE      180
#define ANGLE_STEP     5    // we set the scan to move 5 degrees each time
#define SCAN_DELAY_MS  5

// variables defined in the main
extern volatile RadarState_t CurrentState;
extern int16_t current_angle;
extern int8_t angle_direction;

extern volatile bool capture_done;      // variable used in the part of the sensor to inform that if the capture has been done or not

void run_state_move_servo(Graphics_Context *ctx) {

    // check if the current angle has gone out of bounds and set the direction to move the other way

    if (current_angle >= MAX_ANGLE) {

        angle_direction = -ANGLE_STEP;

        clearRadarMap(ctx);     // clear the map to make it ready to view the results of the next scan, contains hardware-dependent code

    } else if(current_angle <= MIN_ANGLE){

        angle_direction = ANGLE_STEP;

        clearRadarMap(ctx);     // clear the map to make it ready to view the results of the next scan, contains hardware-dependent code
    }

    Servo_Set_Angle(current_angle);     // call the function to move the servo to the next angle, contains hardware-dependent code

    __delay_cycles(3000 * SCAN_DELAY_MS);   // add a little delay to give it time to perform all the operations before moving on the next one

    CurrentState = STATE_TRIGGER_SENSOR;    // update to the next state of the FSM
}

void run_state_trigger_sensor(){
    sensor_trigger();               // contains hardware-dependent code
    CurrentState = STATE_WAIT_ECHO;
}

void run_state_wait_echo() {

    // if the capture has yet to be done, we enter in low-power mode, so the board will consume less energy and will "wake up" only when it will
    // be interrupted again. The interrupt will occur either from the timeout (if the object is too distant) or from the echo signal (if there
    // is an object near the sensor)
    if (!capture_done) {
        MAP_PCM_gotoLPM0();
    }

    if (capture_done) {
        CurrentState = STATE_PROCESS_DATA;
    }
}

void run_state_process_data() {

    current_angle += angle_direction;   // update the angle to move on to the next scan

    CurrentState = STATE_UPDATE_DISPLAY;
}

void run_state_update_display(Graphics_Context *ctx) {
    // calculate the distance of the object scanned, if it has not been scanned anything in the radius of the scanner,
    // it will be set to an agreed value, contains hardware-dependent code
    uint32_t dist = sensor_calculate_distance_cm();

    drawRadar(ctx, current_angle, dist);    // update the map of the radar to the current angle, contains hardware-dependent code

    updateUI(ctx, dist);     // update the distance on the up-right part of the map, contains hardware-dependent code

    UART_IoT_SendData(current_angle, (int)dist);    // send the data to the IoT module to process them, contains hardware-dependent code

    CurrentState = STATE_MOVE_SERVO;    // update the state of the FSM
}
