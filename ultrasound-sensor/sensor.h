#ifndef SENSOR_H_
#define SENSOR_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------
// 1. DEFINIZIONI HARDWARE
// -----------------------------------------------------------------

// Configurazione dei pin per HY-SRF05 (Collegamento a J4.35 e J4.36)
// TRIG: J4.35 -> P6.7 (Output)
#define TRIG_PORT       GPIO_PORT_P6
#define TRIG_PIN        GPIO_PIN7
// ECHO: J4.36 -> P6.6 (Input/Timer A0 CCR1)
#define ECHO_PORT       GPIO_PORT_P6
#define ECHO_PIN        GPIO_PIN6

// Definizioni Timer per Input Capture (P6.6 = TA0.1)
#define TIMER_BASE              TIMER_A0_BASE
#define TIMER_CAPTURE_REGISTER  TIMER_A_CAPTURECOMPARE_REGISTER_1
#define INT_TIMER_N             INT_TA0_N // Interrupt Vector

// -----------------------------------------------------------------
// 2. VARIABILI GLOBALI
// -----------------------------------------------------------------

// Variabili globali per la misurazione (volatile perché modificate dall'ISR)
extern volatile uint32_t t_start;
extern volatile uint32_t t_end;
extern volatile bool capture_done;

// -----------------------------------------------------------------
// 3. PROTOTIPI DELLE FUNZIONI
// -----------------------------------------------------------------

/**
 * @brief Configura i pin GPIO e il Timer A0 per la misurazione ad ultrasuoni.
 */
void sensor_init(void);

/**
 * @brief Invia l'impulso di trigger (HIGH per 10us) al sensore per avviare la misurazione.
 */
void sensor_trigger(void);

/**
 * @brief Calcola la distanza in centimetri a partire dal tempo di volo (ToF) catturato.
 * @return Distanza in cm (max 400cm). Ritorna 999 in caso di errore/timeout.
 */
uint32_t sensor_calculate_distance_cm(void);

/**
 * @brief Interrupt Service Routine per la cattura del Timer A0 (CCR1).
 */
void TA0_N_IRQHandler(void);


#endif /* SENSOR_H_ */
