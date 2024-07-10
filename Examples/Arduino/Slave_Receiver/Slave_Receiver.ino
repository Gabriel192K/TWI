#include <TWI.h>
#include <UART.h>
#include <UART_Terminal.h>

/* Macros */
#define TWI_SLAVE_ADDRESS (const uint8_t)0x0A

/* Instances */
UART_Terminal terminal(&UART);

/* Prototypes */
void rxCallback(const uint8_t size);

void setup(void)
{
    terminal.begin(115200);
    terminal.print("\tTWI - Slave Receiver\n");
    TWI.begin(TWI_SLAVE_ADDRESS);
    TWI.setRxCallback(rxCallback);
}

void loop(void)
{
}

void rxCallback(const uint8_t size)
{
    while (TWI.available())
        terminal.print((const char)TWI.read());
    terminal.println();
}