/* Dependencies */
#include <TWI.h>
#include <UART.h>
#include <UART_Terminal.h>

/* Macros */
#define TWI_FREQUENCY     (const uint32_t)400000
#define TWI_SLAVE_ADDRESS (const uint8_t)0x0A

/* Instances */
UART_Terminal terminal(&UART);

/* Variables */
const uint8_t message[11] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};

void setup(void)
{
    terminal.begin(115200);
    terminal.print("\tTWI - Master Sender\n");
    TWI.begin(TWI_FREQUENCY);
}

void loop(void)
{
    terminal.print("Sending: ");
    for (uint8_t i = 0; i < sizeof(message); i++)
    {
        terminal.print((const char)message[i]);
    }
    terminal.println();

    TWI.beginTransmission(TWI_SLAVE_ADDRESS);
    TWI.write(message, sizeof(message));
    TWI.endTransmission();
    delay(1000);
}
