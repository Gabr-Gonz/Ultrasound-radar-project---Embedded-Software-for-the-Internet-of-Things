#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdio.h>

Timer_A_PWMConfig pwmConfig ={
    TIMER_A_CLOCKSOURCE_SMCLK,              // 3MHz clock
    TIMER_A_CLOCKSOURCE_DIVIDER_1,          // no divider
    60000,                                  // 20ms period (50Hz)
    TIMER_A_CAPTURECOMPARE_REGISTER_2,      // CCR2 is mapped to P2.5
    TIMER_A_OUTPUTMODE_RESET_SET,           // Reset/Set output mode
    3000                                    // initial duty cycle (0 deg)
};

void main(void){
    // stop watchdog timer
    MAP_WDT_A_holdTimer();

    // pin selection: P2.5 corresponds to TA0.2 (Timer_A0, CCR2). Set as primary module function for PWM output.
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5,
                                                    GPIO_PRIMARY_MODULE_FUNCTION);

    printf("Servo Test on P2.5 Starting...\n");

    while(1){
        // move to 0 degrees
        printf("[SERVO] Position: 0 degrees\n");
        pwmConfig.dutyCycle = 3000;
        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
        __delay_cycles(6000000); // 2s pause at 3MHz

        // move to 90 Degrees
        printf("[SERVO] Position: 90 degrees\n");
        pwmConfig.dutyCycle = 4500;
        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
        __delay_cycles(6000000);

        // move to 180 Degrees
        printf("[SERVO] Position: 180 degrees\n");
        pwmConfig.dutyCycle = 6000;
        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
        __delay_cycles(6000000);
    }
}
