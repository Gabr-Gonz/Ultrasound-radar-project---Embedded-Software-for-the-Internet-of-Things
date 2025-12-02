#include "sensor.h"

// -----------------------------------------------------------------
// 1. DEFINIZIONE DELLE VARIABILI GLOBALI
// -----------------------------------------------------------------
volatile uint32_t t_start = 0;
volatile uint32_t t_end = 0;
volatile bool capture_done = false;

// Prototipi interni per chiarezza
static void init_ultrasonic_gpio(void);
static void init_timer_capture(void);

// -----------------------------------------------------------------
// 2. FUNZIONE DI INIZIALIZZAZIONE COMPLESSIVA
// -----------------------------------------------------------------

void sensor_init(void) {
    init_ultrasonic_gpio();
    init_timer_capture();
}

// -----------------------------------------------------------------
// 3. IMPLEMENTAZIONE DELLE FUNZIONI INTERNE (static)
// -----------------------------------------------------------------

/**
 * @brief Configura i pin GPIO per il sensore. P6.7 (Trig) Output, P6.6 (Echo) Peripheral Input.
 */
static void init_ultrasonic_gpio(void) {
    // 1. Pin TRIG (P6.7): Output, inizialmente LOW
    MAP_GPIO_setAsOutputPin(TRIG_PORT, TRIG_PIN);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);

    // 2. Pin ECHO (P6.6): Input, funzione periferica per Timer A0 CCR1 (TA0.1)
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(ECHO_PORT, ECHO_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION);
}

/**
 * @brief Configura il Timer A0 per l'Input Capture sul pin ECHO (CCR1).
 */
static void init_timer_capture(void) {
    // Configurazione del Timer A0 in modalità Contatore Up
    Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,          // Assumendo SMCLK a 48MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_1,      // Dividi per 1 (48MHz)
        65535,                              // Periodo massimo
        TIMER_A_TAIE_INT_DISABLE,
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
        TIMER_A_DO_NOT_CLEAR
    };
    MAP_Timer_A_configureUpMode(TIMER_BASE, &upConfig);

    // Configurazione del modulo di cattura (CCR1)
    Timer_A_CaptureModeConfig captureConfig = {
        TIMER_CAPTURE_REGISTER,             // Usiamo CCR1
        TIMER_A_CAPTUREMODE_RISING_EDGE,    // Cattura sul fronte di salita
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,
        TIMER_A_CCR_INPUTPIN_DISCRETE,
        TIMER_A_CCIE_CCR_INTERRUPT_ENABLE,  // Abilita l'Interrupt di cattura
        TIMER_A_OUTPUTMODE_OUTBITVALUE
    };
    MAP_Timer_A_initCapture(TIMER_BASE, &captureConfig);

    // Abilita l'interrupt specifico del Timer A0 CCR1
    MAP_Interrupt_enableInterrupt(INT_TIMER_N);

    // Avvia il timer
    MAP_Timer_A_startCounter(TIMER_BASE, TIMER_A_UP_MODE);
}

// -----------------------------------------------------------------
// 4. FUNZIONI OPERATIVE PUBBLICHE
// -----------------------------------------------------------------

void sensor_trigger(void) {
    // Reset dello stato di cattura
    capture_done = false;

    // Impulso HIGH per 10us
    MAP_GPIO_setOutputHighOnPin(TRIG_PORT, TRIG_PIN);
    // Pausa di 10us. 48MHz -> 480 cicli
    __delay_cycles(480);
    MAP_GPIO_setOutputLowOnPin(TRIG_PORT, TRIG_PIN);
}

uint32_t sensor_calculate_distance_cm(void) {
    uint32_t pulse_duration_counts = t_end - t_start;

    // Controllo di errore
    if (t_end <= t_start || pulse_duration_counts == 0) {
        return 999;
    }

    // Calcolo della distanza in cm (conversione da conteggi a cm)
    // Formula a virgola fissa (assumendo SMCLK=48MHz): D(cm) ≈ (Conteggi * 357) / 1000
    uint32_t distance_cm = (pulse_duration_counts * 357) / 1000;

    // Limita la distanza
    return (distance_cm > 400) ? 400 : distance_cm;
}

// -----------------------------------------------------------------
// 5. GESTORE DI INTERRUPT (ISR)
// -----------------------------------------------------------------

void TA0_N_IRQHandler(void) {
    // Controlla se l'interrupt proviene dal CCR1
    if (MAP_Timer_A_getCaptureCompareInterruptStatus(TIMER_BASE, TIMER_CAPTURE_REGISTER) &
        TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG) {

        uint16_t current_capture = MAP_Timer_A_getCaptureCompareCount(TIMER_BASE,
            TIMER_CAPTURE_REGISTER);

        // Controlla il livello del pin per determinare se è l'inizio o la fine dell'impulso
        if (MAP_Timer_A_getCaptureCompareInput(TIMER_BASE, TIMER_CAPTURE_REGISTER) ==
            TIMER_A_INPUTPIN_STATE_HIGH) {

            // ➡️ Fronte di salita: Registra l'inizio e passa a cattura sul fronte di discesa
            t_start = current_capture;
            MAP_Timer_A_setCaptureMode(TIMER_BASE, TIMER_CAPTURE_REGISTER,
                TIMER_A_CAPTUREMODE_FALLING_EDGE);
        } else {
            // ⬅️ Fronte di discesa: Registra la fine e torna a cattura sul fronte di salita
            t_end = current_capture;
            MAP_Timer_A_setCaptureMode(TIMER_BASE, TIMER_CAPTURE_REGISTER,
                TIMER_A_CAPTUREMODE_RISING_EDGE);

            capture_done = true; // Misurazione completata
        }

        // Pulisce il flag di interrupt
        MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_BASE,
            TIMER_CAPTURE_REGISTER);
    }
}
