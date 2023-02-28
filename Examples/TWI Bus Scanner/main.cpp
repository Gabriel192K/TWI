/*
 * TWI Bus Scanner.cpp
 *
 * Created: 2/14/2023 11:13:12 PM
 * Author : rotes
 */ 

#include "USART/USART.h"
#include "TWI/TWI.h"
#include "Time/Time.h"

int main(void)
{
    USART.begin(115200);
    USART.printP(PSTR("TWI Bus Scanner Example Code\n\n"));
    TWI.begin();
    Time.begin();
    while (1) 
    {
        static time_t timestamp;
        if (Time.millis() - timestamp >= 1000UL)
        {
            USART.printP(PSTR("Scanning...\n"));
            uint8_t nDevices = 0;
            for (uint8_t address = 10; address < 127; address++)
            {
                uint8_t status = TWI.beginTransmission(address);
                TWI.endTransmission();
                if (status)
                {
                    USART.printf("TWI device found at address 0x%X\n", address);
                    nDevices++;
                }
            }
            if (!nDevices)
                USART.printP(PSTR("No TWI device found\n"));
            else
                USART.printP(PSTR("Done\n\n"));
            timestamp = Time.millis();
        }
    }
}

