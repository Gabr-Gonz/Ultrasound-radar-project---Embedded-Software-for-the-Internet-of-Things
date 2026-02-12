#ifndef IOT_WIFI_H_
#define IOT_WIFI_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdio.h>

void UART_IoT_Init(void);
void UART_IoT_SendData(int angle, int distance);

#endif /* IOT_WIFI_H_ */
