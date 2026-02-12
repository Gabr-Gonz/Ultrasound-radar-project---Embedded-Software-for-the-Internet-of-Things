#ifndef SG90_H_
#define SG90_H_

#include <stdint.h>

 // J2.19 on the BoosterPack MKII maps to P2.5 on the MSP432
#define SERVO_PORT      GPIO_PORT_P2
#define SERVO_PIN       GPIO_PIN5

/*
* PWM signal calculations for 50Hz (20ms period) with 3MHz SMCLK:
* timer clock = 3MHz / 32 = 93,750 Hz
* period = 93,750 * 0.02s = 1,875 ticks
 */
#define SERVO_PWM_PERIOD   1875
#define SERVO_MIN_DUTY     94    // 1.0ms -> 0° (SG90 standard)
#define SERVO_MAX_DUTY     188   // 2.0ms -> 180° (SG90 standard)

void Servo_Init(void);
void Servo_Set_Angle(int16_t angle);

#endif /* SG90_H_ */
