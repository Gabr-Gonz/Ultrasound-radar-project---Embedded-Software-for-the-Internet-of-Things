#include "sg90.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void Servo_Init() {
    // tell P2.5 to listen to TIMER_A
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(SERVO_PORT, SERVO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    // "Up Mode" configuration for TIMER_A
    const Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_32,
        SERVO_PWM_PERIOD,
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // turn off TIMER_A0 interrupts, these
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // tasks are not in the cpu concerns
        TIMER_A_SKIP_CLEAR
    };
    // setting the configurations 
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    // struct to manage the translation of angle of rotation into electric pulse
    const Timer_A_CompareModeConfig outConfig = {
        TIMER_A_CAPTURECOMPARE_REGISTER_2,     
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, 
        TIMER_A_OUTPUTMODE_RESET_SET,           
        SERVO_MIN_DUTY
    };
    // setting the configurations
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &outConfig);

    // start Timer_A
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void Servo_Set_Angle(int16_t angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    // computing the angle as time taken
    uint32_t duty = SERVO_MIN_DUTY + (uint32_t)((angle * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)) / 180);

    MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty);
}
