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

static uint32_t start_wait_time = 0;    // variable used to check if the object is too far

void run_state_move_servo(Graphics_Context *ctx) {

    // check if the current angle has gone out of bounds and set the direction to move the other way

    if (current_angle >= MAX_ANGLE) {

        angle_direction = -ANGLE_STEP;

        clearRadarMap(ctx);     // clear the map to make it ready to view the results of the next scan

    } else if(current_angle <= MIN_ANGLE){

        angle_direction = ANGLE_STEP;

        clearRadarMap(ctx);     // clear the map to make it ready to view the results of the next scan
    }

    Servo_Set_Angle(current_angle);     // call the function to move the servo to the next angle

    __delay_cycles(3000 * SCAN_DELAY_MS);   // add a little delay to give it time to perform all the operations before moving on the next one

    CurrentState = STATE_TRIGGER_SENSOR;    // update to the next state of the FSM
}

void run_state_trigger_sensor(){
    sensor_trigger();
    start_wait_time = TIMER_A1->R; // takes note of the start time of the trigger, useful to see if the object is too far
    CurrentState = STATE_WAIT_ECHO;
}

void run_state_wait_echo() {
    uint32_t current_time = TIMER_A1->R;
    uint32_t elapsed;

    // calculate the time while checking for the overflow of the timer register
    if (current_time >= start_wait_time) {
        elapsed = current_time - start_wait_time;
    } else {
        elapsed = (0xFFFF - start_wait_time) + current_time;
    }

    if (capture_done) {
        CurrentState = STATE_PROCESS_DATA;
    }
   /*
     the maximum distance that the sensor sees is 4 meters, so since our clock runs at 3MHz, 1 tick = 0.33us,
     and since the time for the sound to travel 8 meters (4 meters there and 4 back) is 23.5 ms, we will wait
     for time = 23500 / 0.33 = 70000 more or less
   */
    else if (elapsed > 70000) {
        t_diff = 0;          // no object seen
        capture_done = true; // force the end of the capture
        MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION); // reset the ECHO interrupt to up to be ready for the next scan
        CurrentState = STATE_PROCESS_DATA;
    }
}

void run_state_process_data() {

    current_angle += angle_direction;   // update the angle to move on to the next scan

    CurrentState = STATE_UPDATE_DISPLAY;
}

void run_state_update_display(Graphics_Context *ctx) {

    // calculate the distance of the object scanned, 
    // if it has not been scanned anything in the radius of the scanner, 
    // it will be set to an agreed value
    uint32_t dist = sensor_calculate_distance_cm();     

    // update the map of the radar to the current angle
    drawRadar(ctx, current_angle, dist);    

    // update the distance on the up-right part of the map
    updateUI(ctx, current_angle, dist);     

    // send the data to the IoT module to process them
    UART_IoT_SendData(current_angle, (int)dist);    

    CurrentState = STATE_MOVE_SERVO;    // update the state of the FSM
}
