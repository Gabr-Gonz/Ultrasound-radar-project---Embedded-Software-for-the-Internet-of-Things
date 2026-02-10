#include "sensor.h"

volatile uint32_t t_start = 0;      // variable used to keep track of the starting time of the scan
volatile uint32_t t_diff = 0;       // variable used to calculate the difference between the starting and the ending time of the scan
volatile bool capture_done = false; // variable used to check if the capture has been done or not

static void init_ultrasonic_gpio(void){
    // set TRIG as output and set it on low to not send a false signal to the sensor
    MAP_GPIO_setAsOutputPin(TRIG_PORT, TRIG_PIN);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);

    // set the ECHO pin as input and, since the sensor works on 5V, we use the pull down to keep the signal clean when the sensor is not sending
    // any meaningful information
    MAP_GPIO_setAsInputPinWithPullDownResistor(ECHO_PORT, ECHO_PIN);

    // set the interrupt on port 3 for the ECHO signal of the sensor
    MAP_GPIO_clearInterruptFlag(ECHO_PORT, ECHO_PIN);
    MAP_GPIO_enableInterrupt(ECHO_PORT, ECHO_PIN);

    // set the system to be interrupted and begin his function when the signal will go from low to high
    MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
}

static void init_timer_counter(void){
    // set the timer A1 to be used for the sensor, we use the A1 timer since the A0 is being used by the servo
    const Timer_A_ContinuousModeConfig contConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,          // we use a 3MHz rate
        TIMER_A_CLOCKSOURCE_DIVIDER_1,      // use the full 3MHz rate without dividing it
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // disable the interrupt that trigger when the timer reached the maximum value, since we do no need it
        TIMER_A_DO_CLEAR                    // we set the timer to begin from 0
    };

    // set the timer A1 to use the continuous mode, meaning it will continue to count until it reaches the maximum value (65 535 = 0xFFFF)
    //  and then it will restart from 0. This is the best option for our project since we only need to keep track of some values
    //  while the timer is counting to compare them
    MAP_Timer_A_configureContinuousMode(TIMER_A1_BASE, &contConfig);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
}

void PORT3_IRQHandler(void){
    // keep track of which pin of port 3 triggered the interrupt, then clear the flag to handle the interrupt and
    // to be ready to receive other interrupts in the future
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    //check if the interrupt was caused by the ECHO signal
    if(status & ECHO_PIN){

        // check if the ECHO pin is high, meaning that the pulse has started, so we need to keep track of the starting time. If that's not the case,
        // it means that the pulse came back, so we need to update the value of the ending time on the "else" part of the code
        if(MAP_GPIO_getInputPinValue(ECHO_PORT, ECHO_PIN)){

            t_start = TIMER_A1->R;  // set the variable equal to the timer register to keep track of the starting time of the capture

            // since we tracked the starting time, we now need to check when the value of the pin will be on low to register the ending time
            MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_HIGH_TO_LOW_TRANSITION);

        } else{
            uint32_t t_end = TIMER_A1->R;   // set the variable equal to the timer register to keep track of the ending time of the capture

            // calculate the difference between the starting time and the ending time, while checking if the register has gone in overflow in the meantime
            t_diff = (t_end >= t_start) ? (t_end - t_start) : (0xFFFF - t_start + t_end);

            capture_done = true;    // update the variable to inform that the capture has been done

            // since we tracked the ending time, we now need to check when the value of the pin will be on high to register the next scan
            MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
        }
    }
}

void sensor_init(void){
    init_ultrasonic_gpio();
    init_timer_counter();

    MAP_Interrupt_enableInterrupt(INT_PORT3);   // enable the interrupt on the controller
}

void sensor_trigger(void){

    capture_done = false;   // set it to false to begin the next capture
    t_diff = 0;             // reset the value to be ready for the next capture

    /*
     * to trigger the sensor, we need the TRIG signal to be active for at least 10 us, so we set the TRIG pin on high and, since
     * our clock runs at 3 MHz, it performs 3 000 000 cycles per second, so we need to wait for an amount of at least 3 000 000 * 0.00001 = 30 cycles
     * to be sure that the sensor will begin to track data, then we can set the pin back on low
     */
    MAP_GPIO_setOutputHighOnPin(TRIG_PORT, TRIG_PIN);
    __delay_cycles(30);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);
}

uint32_t sensor_calculate_distance_cm(void){
    if (t_diff == 0 || t_diff > 60000) return 400; // if the object is out of the range of the sensor, send an error value

    /*
     * since we are using a rate of 3MHz, 1 tick = 0.333 us, and since the speed of sound is 0.034 cm/us and we need to keep track of the time that
     * it takes to go there and come back, we divide this value by 2, so the distance is (t_diff * 0.333 * 0.034) / 2 => t_diff / 176.4
     */
    uint32_t distance_cm = t_diff / 176;

    if (distance_cm < 2) return 400; // if the object is too near to the sensor, send an error value
    return (distance_cm > 400) ? 400 : distance_cm;
}

