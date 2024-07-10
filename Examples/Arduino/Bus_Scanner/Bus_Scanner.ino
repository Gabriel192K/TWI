/* Dependencies */
#include <TWI.h>
#include <UART.h>
#include <UART_Terminal.h>

/* Macros */
#define TWI_FREQUENCY (const uint32_t)400000

/* Instances */
UART_Terminal terminal(&UART);

void setup(void)
{
    terminal.begin(115200);
    terminal.print("\tTWI - Bus Scanner\n");
    TWI.begin(TWI_FREQUENCY);
}

void loop(void)
{
    terminal.print("Scanning TWI bus\n");
    uint8_t numOfDevices = 0;
    for (uint8_t address = 1; address < 127; address++)
    {
        TWI.beginTransmission(address);
        const uint8_t status = TWI.endTransmission();
        switch (status)
        {
            case 0:
                terminal.print("MCU not configured as TWI master\n");
                break;
            case 1:
                terminal.print("TWI device found at address: 0x");
                terminal.println(address, HEX);
                break;
        }
    }
    if (!numOfDevices)
        terminal.print("No TWI devices found\n");
	else
        terminal.print("Done scanning\n");
    delay(1000);
}
