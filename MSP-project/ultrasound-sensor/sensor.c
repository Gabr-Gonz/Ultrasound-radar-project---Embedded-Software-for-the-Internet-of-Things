#include "sensor.h"

volatile uint32_t t_start = 0;
volatile uint32_t t_diff = 0;
volatile bool capture_done = false;

static void init_ultrasonic_gpio(void) {
    // Configura TRIG come output
    MAP_GPIO_setAsOutputPin(TRIG_PORT, TRIG_PIN);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);

    // Configura ECHO come ingresso con Pull-Down (più stabile per i 5V)
    MAP_GPIO_setAsInputPinWithPullDownResistor(ECHO_PORT, ECHO_PIN);

    // Configura l'interrupt sulla Porta 3 per il pin ECHO
    MAP_GPIO_clearInterruptFlag(ECHO_PORT, ECHO_PIN);
    MAP_GPIO_enableInterrupt(ECHO_PORT, ECHO_PIN);

    // Inizialmente configurato per il fronte di salita (inizio impulso)
    MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
}

static void init_timer_counter(void) {
    // Usiamo il Timer A1 come cronometro dedicato per il sensore
    const Timer_A_ContinuousModeConfig contConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // 3MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        TIMER_A_TAIE_INTERRUPT_DISABLE,
        TIMER_A_DO_CLEAR
    };
    // CAMBIATO DA TIMER_A0 A TIMER_A1
    MAP_Timer_A_configureContinuousMode(TIMER_A1_BASE, &contConfig);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
}

// In sensor.c, modifica la ISR così:
void PORT3_IRQHandler(void) {
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if (status & ECHO_PIN) {
        if (MAP_GPIO_getInputPinValue(ECHO_PORT, ECHO_PIN)) {
            t_start = TIMER_A1->R;
            MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_HIGH_TO_LOW_TRANSITION);
        } else {
            uint32_t t_end = TIMER_A1->R;
            // Calcolo del delta tempo con gestione overflow a 16-bit
            t_diff = (t_end >= t_start) ? (t_end - t_start) : (0xFFFF - t_start + t_end);

            // Segnala alla FSM che il dato è PRONTO
            capture_done = true;
            MAP_GPIO_interruptEdgeSelect(ECHO_PORT, ECHO_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
        }
    }
}

void sensor_init(void) {
    init_ultrasonic_gpio();
    init_timer_counter();

    // Abilita l'interrupt nel controller (NVIC)
    MAP_Interrupt_enableInterrupt(INT_PORT3);
}

void sensor_trigger(void) {
    capture_done = false;
    t_diff = 0; // Resetta il valore precedente
    MAP_GPIO_setOutputHighOnPin(TRIG_PORT, TRIG_PIN);
    __delay_cycles(30);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);
}

uint32_t sensor_calculate_distance_cm(void) {
    if (t_diff == 0 || t_diff > 60000) return 400; // Vuoto o fuori portata

    // A 3MHz, 1 tick = 0.333 us.
    // Distanza = (tick * 0.333 * 0.034) / 2 = tick / 176.4
    uint32_t distance_cm = t_diff / 176;

    if (distance_cm < 2) return 400; // Filtro rumore troppo vicino
    return (distance_cm > 400) ? 400 : distance_cm;
}

