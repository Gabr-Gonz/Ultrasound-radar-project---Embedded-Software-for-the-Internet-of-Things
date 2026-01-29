#ifndef SENSOR_H_
#define SENSOR_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------
// 1. DEFINIZIONI HARDWARE (Aggiornate ai Pin Funzionanti)
// -----------------------------------------------------------------

// TRIG: J1.3 -> P3.2 (Output)
#define TRIG_PORT       GPIO_PORT_P3
#define TRIG_PIN        GPIO_PIN2

// ECHO: J1.4 -> P3.3 (Input con Interrupt GPIO)
#define ECHO_PORT       GPIO_PORT_P3
#define ECHO_PIN        GPIO_PIN3

// -----------------------------------------------------------------
// 2. VARIABILI GLOBALI
// -----------------------------------------------------------------

extern volatile uint32_t t_start;
extern volatile uint32_t t_diff;   // Usiamo t_diff direttamente
extern volatile bool capture_done;

// -----------------------------------------------------------------
// 3. PROTOTIPI DELLE FUNZIONI
// -----------------------------------------------------------------

void sensor_init(void);
void sensor_trigger(void);
uint32_t sensor_calculate_distance_cm(void);

// Handler per l'interrupt della Porta 3
void PORT3_IRQHandler(void);

#endif /* SENSOR_H_ */
