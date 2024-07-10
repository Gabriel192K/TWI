#include <TWI.h>
#include <UART.h>
#include <UART_Terminal.h>

/* Macros */
#define TWI_SLAVE_ADDRESS (const uint8_t)0x0A

/* Instances */
UART_Terminal terminal(&UART);

/* Variables */
const uint8_t message[11] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};

/* Prototypes */
void txCallback(void);

void setup(void)
{
    terminal.begin(115200);
    terminal.print("\tTWI - Slave Transmitter\n");
    TWI.begin(TWI_SLAVE_ADDRESS);
    TWI.setTxCallback(txCallback);
}

void loop(void)
{
}

void txCallback(void)
{
    TWI.write(message, sizeof(message));
}