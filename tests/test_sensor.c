#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdio.h>

#define TRIGGER_PORT  GPIO_PORT_P3
#define TRIGGER_PIN   GPIO_PIN2
#define ECHO_PORT     GPIO_PORT_P3
#define ECHO_PIN      GPIO_PIN3

void main(void){
    // stop the watchdog timer
    MAP_WDT_A_holdTimer();

    // configure trig as output
    MAP_GPIO_setAsOutputPin(TRIGGER_PORT, TRIGGER_PIN);
    MAP_GPIO_setOutputLowOnPin(TRIGGER_PORT, TRIGGER_PIN);

    // configure echo as input
    MAP_GPIO_setAsInputPin(ECHO_PORT, ECHO_PIN);

    printf("Ultrasonic Standalone Test Started\n");
    printf("Verify connections: TRIG on P3.2, ECHO on P3.3\n");

    while(1){
        // send 10us trigger pulse,  at 3MHz: 10us = 30 cycles
        MAP_GPIO_setOutputHighOnPin(TRIGGER_PORT, TRIGGER_PIN);
        __delay_cycles(30);
        MAP_GPIO_setOutputLowOnPin(TRIGGER_PORT, TRIGGER_PIN);

        // wait for echo to go high
        while(MAP_GPIO_getInputPinValue(ECHO_PORT, ECHO_PIN) == GPIO_INPUT_PIN_LOW);

        // measure duration while echo is high
        uint32_t duration = 0;
        while(MAP_GPIO_getInputPinValue(ECHO_PORT, ECHO_PIN) == GPIO_INPUT_PIN_HIGH) {
            duration++;
            // small delay to make 'duration' represent roughly 1 microsecond. At 3MHz, 3 cycles is ~1us
            __delay_cycles(3);
        }

        // convert to cm: sound speed is 340m/s. formula: distance = (time * 0.034) / 2  using 58 as a standard divisor for microseconds to cm.

        uint32_t distance = duration / 58;

        if(distance > 0 && distance < 400){
            printf("[SENSOR] Distance: %d cm\n", distance);
        }else{
            printf("[SENSOR] Out of range or no reflection.\n");
        }

        // wait 200ms before next measurement (600 000 cycles at 3MHz)
        __delay_cycles(600000);
    }
}
