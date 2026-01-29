#ifndef SG90_H_
#define SG90_H_

#include <stdint.h>

/* * J2.19 sul BoosterPack MKII corrisponde a P2.5 sulla MSP432.
 * P2.5 ha come funzione primaria l'uscita del Timer_A0, canale CCR2.
 */
#define SERVO_PORT      GPIO_PORT_P2
#define SERVO_PIN       GPIO_PIN5

/* Calcoli per il segnale PWM a 50Hz (periodo 20ms) con SMCLK a 3MHz:
 * Clock Timer = 3MHz / 32 = 93750 Hz
 * Periodo = 93750 * 0.02 = 1875 tick
 */
#define SERVO_PWM_PERIOD   1875
#define SERVO_MIN_DUTY     94    // 1.0ms -> 0 gradi (standard SG90)
#define SERVO_MAX_DUTY     188   // 2.0ms -> 180 gradi (standard SG90)

void Servo_Init(void);
void Servo_Set_Angle(int16_t angle);

#endif /* SG90_H_ */
