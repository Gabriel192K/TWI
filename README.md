# TWI
A lightweight C++ library to control the **TWI(I2C)** bus of an AVR MCU. 

## 📦 Features
- Able to work in both ***master*** and ***slave*** mode of operation.
- Interrupt driven, buffered transmission and reception.
- Callback logic for ***slave*** transmission and reception.

## 🚀 Usage

### Master and Slave
```cpp
/* Dependencies */
#include "TWI.h"

/* Macros */
#define TWI_MASTER_FREQUENCY (const uint32_t)400000
#define TWI_SLAVE_FREQUENCY  (const uint8_t)0x10

/* Prototypes */
void slave_rx_callback(const uint8_t size);
void slave_tx_callback(void);

int main(void)
{
    TWI0.begin(TWI_MASTER_FREQUENCY);
    TWI1.begin(TWI_SLAVE_FREQUENCY);
    
    TWI1.setRxCallback(slave_rx_callback);
    TWI1.setTxCallback(slave_tx_callback);

    while (1)
    {
        TWI0.beginTransmission(0x30);
        TWI0.write(0xFF);
        TWI0.endTransmission();

        TWI0.requestFrom(0x30, 1);
        const uint8_t byte = TWI0.read(); 
    }
    return (0);
}

void slave_rx_callback(const uint8_t size)
{
    while (size)
    {
        uint8_t byte = TWI1.read();
    }
}

void slave_tx_callback(void)
{
    TWI1.write(0xFF);
}
```

### Bus Scanner
```cpp

/* Dependencies */
#include "TWI.h"

/* Macros */
#define TWI_BUS_FREQUENCY (const uint32_t)400000


int main(void)
{
    TWI0.begin(TWI_BUS_FREQUENCY);

    while (1)
    {
        uint8_t devices_found = 0;

        for (uint8_t index = 0; index < 0x7F; index++)
        {
            TWI0.beginTransmission(index);
            const uint8_t status = TWI0.endTransmission();

            switch (status)
            {
                case TW_MT_SLA_ACK:
                    devices_found++;
                    break;
                case TW_MT_ARB_LOST:
                    // Bus arbitration lost as master.
                    break;
                case TW_BUS_ERROR:
                    // Bus general error.
                    break;
                case TW_NO_INFO:
                    // No information about bus state.
                    break;
                default:
                    // Used for safety.
                    break;
            }
        }
    }
    
    return (0);
}
```

## Compatibility
For now it is fully compatible with ***Arduino IDE*** and ***Microchip Studio IDE*** using the standard ***AVR*** devices
***(not XAVR)***.

