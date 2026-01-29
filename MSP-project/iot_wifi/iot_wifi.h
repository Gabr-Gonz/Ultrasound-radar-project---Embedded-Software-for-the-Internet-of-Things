#ifndef IOT_WIFI_H_
#define IOT_WIFI_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Inizializza la porta UART EUSCI_A1 (Pin P2.2 RX, P2.3 TX)
 * per comunicare con il modulo ESP-12E a 9600 baud.
 */
void UART_IoT_Init(void);

/**
 * @brief Invia i dati del radar formattati come "angolo,distanza\n"
 * @param angle Angolo corrente del servo (0-180)
 * @param distance Distanza rilevata in cm (usa -1 per nessun oggetto)
 */
void UART_IoT_SendData(int angle, int distance);

#endif /* IOT_WIFI_H_ */
