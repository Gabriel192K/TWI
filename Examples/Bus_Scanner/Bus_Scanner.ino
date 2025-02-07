/* Dependencies */
#include <TWI.h>  //*< Include the TWI (Two-Wire Interface) library.
#include <UART.h> //*< Include the UART library for serial communication.
#include <Time.h> //*< Include the Time library for time-related functions.

void setup(void)
{
    UART0.begin(115200);                             //*< Initialize UART0 with a baud rate of 115200.
    UART0.print(F("TWI - Bus Scanner\n"));           //*< Print "TWI - Bus Scanner" to the UART0 output (serial monitor).
    TWI.begin((const uint32_t)400000);               //*< Initialize the TWI interface with a 400kHz frequency.
}

void loop(void)
{
    UART0.print(F("Scanning TWI bus...\n"));            //*< Print "Scanning TWI bus..." to UART0 to notify the start of bus scanning.

    uint8_t numOfDevices = 0;                           //*< Initialize a variable to keep track of the number of TWI devices found.
    
    for (uint8_t address = 1; address < 127; address++) //*< Loop through all possible TWI addresses (1 to 126).
    {
        TWI.beginTransmission((const uint8_t)address);  //*< Begin a TWI transmission to the current address.
        const uint8_t status = TWI.endTransmission();   //*< End the TWI transmission and store the result (status).

        switch (status)                                //*< Handle different statuses returned by the endTransmission function.
        {
            case TW_MT_SLA_ACK:                        //*< If the address acknowledged (SLA-ACK), a device is present at the address.
                UART0.print(F("TWI device found at address: "));  //*< Print the address where a device was found.
                UART0.println(address);                //*< Print the address of the found TWI device.
                numOfDevices++;                        //*< Increment the number of devices found.
                break;
            case TW_MT_ARB_LOST:                       //*< If arbitration is lost (bus contention), print an error message.
                UART0.print(F("TWI arbitration lost\n"));  //*< Print that arbitration was lost.
                break;
            case TW_BUS_ERROR:                         //*< If there’s a bus error, print an error message.
                UART0.print(F("TWI bus error detected.\n"));  //*< Print that a bus error was detected.
                break;
            case TW_NO_INFO:                           //*< If no information is available for the address, print that no info was found.
                UART0.print(F("TWI bus no info\n"));   //*< Print that no information was found for the address.
                break;
        }
    }

    if (!numOfDevices)                                //*< If no devices were found on the bus.
        UART0.print(F("No TWI devices found\n"));     //*< Print a message indicating no devices were found.
    else                                              //*< If devices were found.
        UART0.print(F("Done scanning\n"));            //*< Print a message indicating the scan is complete.

    Time.delay(1);                                    //*< Delay for 1 second before starting the next scan.
}
