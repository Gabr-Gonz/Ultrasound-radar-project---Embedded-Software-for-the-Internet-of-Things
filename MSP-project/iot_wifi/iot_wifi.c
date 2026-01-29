#include "iot_wifi.h"

void UART_IoT_Init(void) {
    // Configurazione UART: 9600 baud, SMCLK = 3MHz
    // Calcoli basati sulla tabella baud rate di DriverLib
    const eUSCI_UART_ConfigV1 uartConfig = {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // Sorgente Clock 3MHz
        19,                                      // BRDIV = 19
        8,                                       // UCxBRF = 8
        85,                                      // UCxBRS = 85
        EUSCI_A_UART_NO_PARITY,                  // Nessuna parità
        EUSCI_A_UART_LSB_FIRST,                  // Bit meno significativo per primo
        EUSCI_A_UART_ONE_STOP_BIT,               // 1 bit di stop
        EUSCI_A_UART_MODE,                       // Modalità UART
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling abilitato
    };

    // 1. Configura i pin P2.2 (RX) e P2.3 (TX) come funzione primaria UART
    // J4.33 e J4.34 sulla LaunchPad
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // 2. Inizializza e abilita il modulo EUSCI_A1
    MAP_UART_initModule(EUSCI_A1_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A1_BASE);
}

void UART_IoT_SendData(int angle, int distance) {
    char buffer[32];

    // Validazione dato: se la distanza è fuori portata mandiamo -1
    // per essere compatibili con la logica della pagina Web
    int d = (distance >= 400 || distance <= 0) ? -1 : distance;

    // Formattazione stringa CSV (Comma Separated Values)
    int len = sprintf(buffer, "%d,%d\n", angle, d);

    // Trasmissione della stringa carattere per carattere
    int i;
    for(i = 0; i < len; i++) {
        MAP_UART_transmitData(EUSCI_A1_BASE, buffer[i]);
    }
}
