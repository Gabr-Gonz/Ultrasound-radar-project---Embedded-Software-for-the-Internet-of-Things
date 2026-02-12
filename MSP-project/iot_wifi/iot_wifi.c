#include "iot_wifi.h"

void UART_IoT_Init(void){
    /* * UART Configuration: 9600 baud, SMCLK = 3MHz.
     * These values (BRDIV, UCxBRF, UCxBRS) are calculated based on
     * the Texas Instruments DriverLib baud rate tables for 3MHz clocking.
     */
    const eUSCI_UART_ConfigV1 uartConfig={
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // clock source set to SMCLK=3MHz
        19,                                           // BRDIV = 19
        8,                                            // UCxBRF = 8
        85,                                           // UCxBRS = 85
        EUSCI_A_UART_NO_PARITY,                       // no parity bit
        EUSCI_A_UART_LSB_FIRST,                       // least significant bit first
        EUSCI_A_UART_ONE_STOP_BIT,                    // one stop bit
        EUSCI_A_UART_MODE,                             // standard UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // oversampling enabled for better error resilience
    };

    // configure P2.2 (RX) and P2.3 (TX) pins for Peripheral Module Function. These pins connect to J4.33 and J4.34 on the Boosterpack,
    // which maps respectively to P5.1 and P2.3 on the MSP
     MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // initialize and enable the EUSCI_A1 hardware module. The module is now ready to handle serial communication with the IoT device
    MAP_UART_initModule(EUSCI_A1_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A1_BASE);
}

void UART_IoT_SendData(int angle, int distance){
    char buffer[32];

    // if distance is out of range, we send -1 to remain compatible with the Web dashboard logic.
    int d = (distance >= 400 || distance <= 0) ? -1 : distance;

    // format the data into a CSV string.The format is "angle,distance\n".
    int len = sprintf(buffer, "%d,%d\n", angle, d);

    // Transmission: Send the string character by character over UART. The hardware module handles the serialization of each byte.
    int i;
    for(i = 0; i < len; i++) {
        MAP_UART_transmitData(EUSCI_A1_BASE, buffer[i]);
    }
}
