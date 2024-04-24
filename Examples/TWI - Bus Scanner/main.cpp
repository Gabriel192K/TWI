/*
 * TWI - Bus Scanner.cpp
 *
 * Created: 4/24/2024 9:23:51 PM
 * Author : rotes
 */ 

/* Dependencies */
#include "TWI/TWI.h"
#include "USART/USART.h"

/* Macros */
#define TWI_FREQUENCY  (const uint32_t)400000
#define USART_BAUDRATE (const uint32_t)115200

int main(void)
{
    TWI.begin(TWI_FREQUENCY);
    USART.begin(USART_BAUDRATE);

    USART.print("Scanning TWI bus\n");
    uint8_t amountOfDevices = 0;
	for (uint8_t address = 1; address < 127; address++)
	{
		TWI.beginTransmission(address);
		const uint8_t error = TWI.endTransmission();
        switch (error)
        {
            case 1:
                USART.print("TWI device found at address ");
                USART.println(address);
				amountOfDevices++;
                break;
            case 4:
                USART.print("Unknown error at address ");
                USART.println(address);
                break;
            default:
                break;
        }
	}
  
	if (amountOfDevices == 0)
        USART.print("No TWI devices found\n");
	else
        USART.print("Done scanning\n");
    while (1) 
    {
    }
}

