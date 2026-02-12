#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>

// UART configuration (9600 Baud at 3MHz SMCLK)
const eUSCI_UART_ConfigV1 uartConfig ={
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,
    19, 8, 85,
    EUSCI_A_UART_NO_PARITY,
    EUSCI_A_UART_LSB_FIRST,
    EUSCI_A_UART_ONE_STOP_BIT,
    EUSCI_A_UART_MODE,
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

void main(void){
    MAP_WDT_A_holdTimer();

    // configure P2.2 (RX) and P2.3 (TX) for EUSCI_A1
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_UART_initModule(EUSCI_A1_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A1_BASE);

    printf("UART Test Started\n");

    while(1){
        char buffer[32];
        int len = sprintf(buffer, "45,120\n");

        int i;
        for(i = 0; i < len; i++){
            MAP_UART_transmitData(EUSCI_A1_BASE, buffer[i]);
        }

        printf("Sent: %s", buffer);

        //  Execution pause 3 000 000 cycles = 1 second at 3MHz
        __delay_cycles(3000000);
    }
}
