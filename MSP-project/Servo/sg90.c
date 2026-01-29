#include "sg90.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void Servo_Init() {
    // 1. Configura P2.5 (J2.19) come uscita per il modulo periferico (Timer PWM)
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(SERVO_PORT, SERVO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    // 2. Configura il Timer_A0 in modalità "Up Mode"
    const Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_32,
        SERVO_PWM_PERIOD,
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Macro aggiornata
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
        TIMER_A_SKIP_CLEAR
    };
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    // 3. Configura il canale CCR2 per il PWM (NOTA: Nome struttura corretto)
    const Timer_A_CompareModeConfig outConfig = {
        TIMER_A_CAPTURECOMPARE_REGISTER_2,      // Canale CCR2
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // Interrupt del registro spento
        TIMER_A_OUTPUTMODE_RESET_SET,           // Modalità PWM standard
        SERVO_MIN_DUTY
    };
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &outConfig);

    // 4. Avvia il conteggio del Timer_A0
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void Servo_Set_Angle(int16_t angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    uint32_t duty = SERVO_MIN_DUTY + (uint32_t)((angle * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)) / 180);

    MAP_Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, duty);
}
