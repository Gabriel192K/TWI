/*
 * TWI - Slave - Read and Write.cpp
 *
 * Created: 4/22/2024 3:51:22 PM
 * Author : rotes
 */ 

/* Dependencies */
#include "TWI/TWI.h"

/* Macros */
#define TWI_SLAVE_ADDRESS (const uint8_t)0x0A

/* Prototypes */
void rxCallback(uint8_t* buffer, const uint8_t size);
void txCallback(void);

int main(void)
{
    DDRD |= (1 << 2);
    TWI.begin(TWI_SLAVE_ADDRESS);
    TWI.setRxCallback(rxCallback);
    TWI.setTxCallback(txCallback);
    while (1)
    {
    }
}

void rxCallback(uint8_t* buffer, const uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
        buffer[i] = 0;
}

void txCallback(void)
{

}
