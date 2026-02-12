#include "sg90.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void Servo_Init() {
    /*
     * configures the pin to be controlled by the hardware timer (PWM) instead of standard software I/O.This allows the servo to move using the
     *  PWM technique: the timer starts counting from 0 (with the pin set to high) until it reaches a specific value which varies depending on
     *  the desired angle. Once this value is reached, the pin is set to low, so based on the duration the pin remains high (called the duty cycle),
     *  the servo determines the angle it must rotate to in the next step.
     *  */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(SERVO_PORT, SERVO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    // configure Timer_A0 to Up Mode, which will count from 0 to a desired value and then (in our case) will set a pin to low. After that, it will
    // start again to count from 0
    const Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,              // we use a 3MHz rate
        TIMER_A_CLOCKSOURCE_DIVIDER_32,         // divides the SMCLK frequency by 32. Since the servo needs a 50Hz signal (20ms period), the timer frequency becomes 3 000 000 / 32 = 93 750 ticks per second
        SERVO_PWM_PERIOD,                       // set to 1875. This is calculated as: 93 750 ticks/sec * 0.020 sec = 1 875 ticks. This defines the total duration of one PWM cycle
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // disable the interrupt that would occur when the timer restarts from 0, since we do not need it
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // disable the interrupt of CCR0, since we do not need it
        TIMER_A_SKIP_CLEAR                      // do not start to count when it will be configured, but wait until we say to do so
    };
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    // configure the CCR2 channel for the PWM
    const Timer_A_CompareModeConfig outConfig = {
        TIMER_A_CAPTURECOMPARE_REGISTER_2,          // CCR2 channel
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // disable the interrupt of the capture compare, since we do not need it
        TIMER_A_OUTPUTMODE_RESET_SET,               // set the PWM mode: when the timer starts from 0, the pin will be high, when it reaches the desired value, it will be set to low and restart again
        SERVO_MIN_DUTY                              // the capture value, which in the beginning is set to 94, since based on the standard of the SG90 we need a 1 ms pulse if we want to start from 0°, which is 93 750 / 1000 = 93.75 ~ 94
    };
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &outConfig);

    // tell the timer to start counting
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void Servo_Set_Angle(int16_t angle) {
    // safe check to not pass the limit of rotation
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    // formula to compute the next comparison value for the register, mapping the desired scan angle to the servo's duty cycle
    // based on SG90 standards. (SERVO_MAX_DUTY - SERVO_MIN_DUTY) indicates the total number of ticks within which the servo can move
    // (the operational range).  The expression ((angle * range) / 180) calculates the proportional offset for the  current angle
    // (e.g., 90° is 50% of the range). Finally, we add SERVO_MIN_DUTY  to align this offset with the 1ms (0°) base required by the SG90.
    uint32_t duty = SERVO_MIN_DUTY + (uint32_t)((angle * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)) / 180);

    // set the capture just computed
    MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty);
}
