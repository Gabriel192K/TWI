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
            case TW_MT_SLA_ACK:
                terminal.print("TWI device found at address: 0x");
                terminal.println(address, HEX);
                numOfDevices++;
                break;
            case TW_MT_ARB_LOST:
                terminal.print("TWI arbitration lost\n");
                break;
            case TW_BUS_ERROR:
                terminal.print("TWI bus error detected\n");
                break;
            case TW_NO_INFO:
                terminal.print("TWI bus no info\n");
                break;
        }
    }
    if (!numOfDevices)
        terminal.print("No TWI devices found\n");
	else
        terminal.print("Done scanning\n");
    delay(1000);
}
