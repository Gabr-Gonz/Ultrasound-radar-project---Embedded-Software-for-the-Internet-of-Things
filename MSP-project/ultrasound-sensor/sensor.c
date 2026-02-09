#include "sensor.h"

volatile uint32_t t_start = 0;      // variable used to keep track of the starting time of the scan
volatile uint32_t t_diff = 0;       // variable used to keep track of the ending time of the scan, used with the starting time it will be useful to calculate the distance of the object that has been scanned
volatile bool capture_done = false; // variable used to check if the capture has been done or not

static void init_ultrasonic_gpio(void){
    // set TRIG as output
    MAP_GPIO_setAsOutputPin(TRIG_PORT, TRIG_PIN);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);

    // Configura ECHO come ingresso con Pull-Down (più stabile per i 5V)
    MAP_GPIO_setAsInputPinWithPullDownResistor(ECHO_PORT, ECHO_PIN);

    // set the interrupt on port 3 for the ECHO signal of the sensor

    MAP_GPIO_clearInterruptFlag(ECHO_PORT, ECHO_PIN);
    MAP_GPIO_enableInterrupt(ECHO_PORT, ECHO_PIN);

    // Inizialmente configurato per il fronte di salita (inizio impulso)
    MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
}

static void init_timer_counter(void){
    // set the timer A1 to be used for the sensor, we use the A1 timer since the A0 is being used by the servo
    const Timer_A_ContinuousModeConfig contConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // we use a 3MHz rate
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        TIMER_A_TAIE_INTERRUPT_DISABLE,
        TIMER_A_DO_CLEAR
    };

    // set the timer A1 to use the continous mode
    MAP_Timer_A_configureContinuousMode(TIMER_A1_BASE, &contConfig);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
}

void PORT3_IRQHandler(void){
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if(status & ECHO_PIN){
        if(MAP_GPIO_getInputPinValue(ECHO_PORT, ECHO_PIN)){
            t_start = TIMER_A1->R;
            MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_HIGH_TO_LOW_TRANSITION);
        } else{
            uint32_t t_end = TIMER_A1->R;

            t_diff = (t_end >= t_start) ? (t_end - t_start) : (0xFFFF - t_start + t_end);   // calculate the difference between the starting time and the ending time while checking if this operation ended in an overflow


            capture_done = true;
            MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
        }
    }
}

void sensor_init(void){
    init_ultrasonic_gpio();     // hardware bound part of code
    init_timer_counter();       // hardware bound part of code

    MAP_Interrupt_enableInterrupt(INT_PORT3);   // enable the interrupt on the controller
}

void sensor_trigger(void){

    capture_done = false;
    t_diff = 0; // Resetta il valore precedente
    MAP_GPIO_setOutputHighOnPin(TRIG_PORT, TRIG_PIN);
    __delay_cycles(30);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);
}

uint32_t sensor_calculate_distance_cm(void){
    if (t_diff == 0 || t_diff > 60000) return 400; // if the object is out of the range of the sensor, send an errore value

    // since we are using a rate of 3MHz, 1 tick = 0.333 us, so the distance is (tick * 0.333 * 0.034) / 2 = tick / 176.4
    // Distanza = (tick * 0.333 * 0.034) / 2 = tick / 176.4
    uint32_t distance_cm = t_diff / 176;

    if (distance_cm < 2) return 400; // Filtro rumore troppo vicino
    return (distance_cm > 400) ? 400 : distance_cm;
}

