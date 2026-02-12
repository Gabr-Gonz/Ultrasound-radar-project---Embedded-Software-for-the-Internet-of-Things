#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

// costants
#define MIN_ANGLE      0
#define MAX_ANGLE      180
#define ANGLE_STEP     5
#define TIMEOUT_TICKS  70000  // ~23.3ms at 3MHz, max range of 4 meters round-trip

// global variables
int16_t current_angle = 0;
int8_t angle_direction = 5;      // step size and direction (positive = clockwise)
bool capture_done = false;       // set by echo interrupt 
uint32_t start_wait_time = 0;    // timestamp when the trigger was sent
uint32_t current_simulated_time = 0; // simulated timer register
uint32_t distance = 0;
uint32_t t_diff = 0;             // time of flight (in ticks)

// finite state machine definitions
typedef enum {
    STATE_MOVE_SERVO = 0,        // update servo position
    STATE_TRIGGER_SENSOR,        // send ultrasonic pulse
    STATE_WAIT_ECHO,             // wait for reflection or timeout
    STATE_PROCESS_DATA,          // calculate distance and next angle
    STATE_UPDATE_DISPLAY         // refresh UI and send IoT data
} RadarState_t;

RadarState_t CurrentState;

// function prototypes
void run_state_move_servo();
void run_state_trigger_sensor();
void run_state_wait_echo();
void run_state_process_data();
void run_state_update_display();

// mock hardware functions
void Servo_Set_Angle(int16_t angle){ printf("\n[SERVO] Angle: %d", angle); }
void sensor_trigger(){ printf("\n[SENSOR] Trigger sent!"); }
void clearRadarMap(){ printf("\n[LCD] Limit reached: Map Cleared"); }
void drawRadar(int a, int d){ printf("\n[LCD] Rendering: %d deg, %d cm", a, d); }

int main(void){
    CurrentState = STATE_MOVE_SERVO;
    int cycles = 0;

    printf("=== STARTING INTEGRATED RADAR LOGIC TEST ===\n");

    // run the FSM for a fixed number of iterations to observe behavior
    while(cycles < 300){ 
        switch(CurrentState) {
            case STATE_MOVE_SERVO:    run_state_move_servo(); break;
            case STATE_TRIGGER_SENSOR: run_state_trigger_sensor(); break;
            case STATE_WAIT_ECHO:      run_state_wait_echo(); break;
            case STATE_PROCESS_DATA:   run_state_process_data(); break;
            case STATE_UPDATE_DISPLAY: run_state_update_display(); break;
        }
        cycles++;
    }
    printf("\n\n=== TEST SUITE FINISHED ===\n");
    return 0;
}

// state implementations


// handles servo rotation logic, flipping direction at 0 and 180 degrees.

void run_state_move_servo(){
    if (current_angle >= MAX_ANGLE) {
        angle_direction = -ANGLE_STEP; // reverse to counter-clockwise
        clearRadarMap();               // clear display for the new scan
    } else if(current_angle <= MIN_ANGLE) {
        angle_direction = ANGLE_STEP;  // reverse to clockwise
        clearRadarMap();
    }
    
    sleep(3);                      // pause to see the results
    
    Servo_Set_Angle(current_angle);
    CurrentState = STATE_TRIGGER_SENSOR;
}

// triggers the sensor and prepares for echo capture
void run_state_trigger_sensor(){
    sensor_trigger();
    start_wait_time = current_simulated_time; // record start timestamp
    capture_done = false;                     // reset flag for new measurement
    CurrentState = STATE_WAIT_ECHO;
}

// monitors the echo signal. Implements timeout logic to prevent FSM hang.
void run_state_wait_echo(){
    uint32_t elapsed;

    // SIMULATION LOGIC: let's pretend there's an object at 120cm 
    // but only when the radar is pointing between 20 and 40 degrees.
    bool fake_object_present = (current_angle >= 20 && current_angle <= 40);
    uint32_t fake_echo_time = 120 * 176; // 120cm * 176 ticks/cm = 21120 ticks

    // increment time for the simulations
    current_simulated_time += 2000; 

    if (current_simulated_time >= start_wait_time) {
        elapsed = current_simulated_time - start_wait_time;
    } else {
        elapsed = (0xFFFFFFFF - start_wait_time) + current_simulated_time;
    }

    // if the object is present and the "time of flight" has passed
    if (fake_object_present && elapsed >= fake_echo_time) {
        capture_done = true; 
        printf("\n[ECHO] Object detected at angle %d!", current_angle);
        t_diff = elapsed; 
        CurrentState = STATE_PROCESS_DATA;
    }
    // no object or we haven't hit the fake object's distance
    else if (elapsed > TIMEOUT_TICKS) {
        printf("\n[TIMEOUT] Max range exceeded (Elapsed: %u).", elapsed);
        t_diff = 0;
        capture_done = true;
        CurrentState = STATE_PROCESS_DATA;
    }
}

// calculates physical distance and updates the scanning angle.
void run_state_process_data(){
    if (t_diff > 0) {
        distance = t_diff / 176; 
    } else {
        distance = 400; // default value for "out of range" 
    }
    
    current_angle += angle_direction; // advance the servo angle
    CurrentState = STATE_UPDATE_DISPLAY;
}

// triggers UI updates and sends data to the IoT module.
void run_state_update_display(){
    drawRadar(current_angle, distance);
    
    // in a real scenario, this sends the CSV string to the ESP12-E
    // UART_IoT_SendData(current_angle, distance);
    
    CurrentState = STATE_MOVE_SERVO; // Restart the cycle
}