/* Dependencies */
#include <TWI.h>
#include <UART.h>
#include <UART_Terminal.h>

/* Macros */
#define TWI_FREQUENCY     (const uint32_t)400000
#define TWI_SLAVE_ADDRESS (const uint8_t)0x0A

/* Instances */
UART_Terminal terminal(&UART);

void setup(void)
{
    terminal.begin(115200);
    terminal.print("\tTWI - Master Receiver\n");
    TWI.begin(TWI_FREQUENCY);
}

void loop(void)
{
    terminal.print("Reading: ");
    TWI.requestFrom(TWI_SLAVE_ADDRESS, 11, 1);
    while (TWI.available())
        terminal.print((const char)TWI.read());
    terminal.println();
    delay(1000);
}
