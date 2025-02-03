/* Dependencies */
#include <TWI.h>
#include <UART.h>

void setup(void)
{
    UART0.begin(115200);
    UART0.print(F("TWI - Bus Scanner\n"));
    TWI.begin((const uint32_t)400000);
}

void loop(void)
{
    UART0.print(F("Scanning TWI bus...\n"));

    uint8_t numOfDevices = 0;
    for (uint8_t address = 1; address < 127; address++)
    {
        TWI.beginTransmission((const uint8_t)address);
        const uint8_t status = TWI.endTransmission();
        switch (status)
        {
            case TW_MT_SLA_ACK:
                UART0.print(F("TWI device found at address: "));
                UART0.println(address);
                numOfDevices++;
                break;
            case TW_MT_ARB_LOST:
                UART0.print(F("TWI arbitration lost\n"));
                break;
            case TW_BUS_ERROR:
                UART0.print(F("TWI bus error detected.\n"));
                break;
            case TW_NO_INFO:
                UART0.print(F("TWI bus no info\n"));
                break;
        }
    }

    if (!numOfDevices)
        UART0.print(F("No TWI devices found\n"));
	else
        UART0.print(F("Done scanning\n"));
    delay(1000);
}