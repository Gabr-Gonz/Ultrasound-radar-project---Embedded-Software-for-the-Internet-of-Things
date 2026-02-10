#ifndef SENSOR_H_
#define SENSOR_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

// TRIG: J1.3 on the boosterpack, which is P3.2 on the MSP (output)
#define TRIG_PORT       GPIO_PORT_P3
#define TRIG_PIN        GPIO_PIN2

// ECHO: J1.4 on the boosterpack, which is P3.3 on the MSP (input with interrupt)
#define ECHO_PORT       GPIO_PORT_P3
#define ECHO_PIN        GPIO_PIN3

extern volatile uint32_t t_start;
extern volatile uint32_t t_diff;
extern volatile bool capture_done;

void sensor_init(void);
void sensor_trigger(void);
uint32_t sensor_calculate_distance_cm(void);
void PORT3_IRQHandler(void);

#endif /* SENSOR_H_ */
