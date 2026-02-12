#ifndef SG90_H_
#define SG90_H_

#include <stdint.h>

/* * J2.19 on the BoosterPack MKII, which is P2.5 on MSP432.
 * P2.5 has Timer_A0 output as main purpose, using CCR2.
 */
#define SERVO_PORT      GPIO_PORT_P2
#define SERVO_PIN       GPIO_PIN5

/* Calcutations for the 50Hz PWM signal (20ms period) with SMCLK at 3MHz:
 * Clock Timer = 3MHz / 32 = 93750 Hz
 * Period = 93750 * 0.02 = 1875 tick
 */
#define SERVO_PWM_PERIOD   1875	 // 1875 clock ticks
#define SERVO_MIN_DUTY     94    // 1.0ms -> 0 degrees (standard SG90)
#define SERVO_MAX_DUTY     188   // 2.0ms -> 180 degrees (standard SG90)

void Servo_Init(void);
void Servo_Set_Angle(int16_t angle);

#endif /* SG90_H_ */
