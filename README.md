# TWI
- TWI library for TWI communication protocol

## Key features
- Compatible with `Microchip Studio IDE` & `Arduino IDE`
- ```begin()``` and ```end()``` functions to easily enable or disable the `TWI` bus
- Interrupt and machine state driven logic for both transmission and reception as `master`
- Callbacks for both transmission and reception as `slave`

## Tested on
- `Microchip Studio IDE` and `Arduino IDE` with `ATmega328P` and @400kHz bus frequency

## Example - Master transmission and reception
```C
/* Dependencies */
#include "TWI\TWI.h"

/* Macros */
#define MASTER_FREQUENCY (const uint32_t)400000
#define SLAVE_ADDRESS    (const uint8_t)0x27

/* Variables */
const uint8_t b = 10;
const uint8_t array[8] = {1, 2, 3, 4, 5, 6, 7, 8};
struct Structure
{
    const uint32_t dword = 123456;
    const float f = 3.14;
    const char message[11] = "Hello World";
};
Structure structure;

int main(void)
{
    TWI.begin(MASTER_FREQUENCY);
    TWI.beginTransmission(SLAVE_ADDRESS);
    TWI.write(b);
    TWI.write(array, sizeof(array));
	TWI.endTransmission();
	TWI.beginTransmission(SLAVE_ADDRESS);
    TWI.write(&structure, sizeof(structure));
    TWI.endTransmission();

    const uint8_t amountRequested = TWI.requestFrom(SLAVE_ADDRESS, (const uint8_t)8);
    const uint8_t amountReceived = TWI.available();
    if (!amountReceived)
    {
        // Handle errors here
    }

    while (TWI.available())
    {
        const uint8_t byte1 = TWI.read();
        // Read bytes 
    }
}
```
