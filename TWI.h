#ifndef __TWI_H__
#define __TWI_H__

/* Dependecies */
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/atomic.h>
#include <util/delay.h>

#define TWI_DEFAULT_FREQUENCY (const uint32_t)400000
#define TWI_ROLE_MASTER       (const uint8_t)0
#define TWI_ROLE_SLAVE        (const uint8_t)1
#define TWI_READY             (const uint8_t)0
#define TWI_MRX               (const uint8_t)1
#define TWI_MTX               (const uint8_t)2
#define TWI_SRX               (const uint8_t)3
#define TWI_STX               (const uint8_t)4
#define TWI_BUFFER_SIZE       (const uint8_t)32
#define TWI_BEGIN             ((1 << TWEN) | (1 << TWIE) | (1 << TWEA))
#define TWI_SEND_ACK          ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA))
#define TWI_SEND_NACK         ((1 << TWEN) | (1 << TWIE) | (1 << TWINT))
#define TWI_SEND_START        ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA) | (1 << TWSTA))
#define TWI_SEND_REP_START    ((1 << TWEN) | (1 << TWINT) | (1 << TWSTA))
#define TWI_SEND_STOP         ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA) | (1 << TWSTO))
#define TWI_END               (const uint8_t)0

/**
 * @brief Class for managing TWI (Two-Wire Interface) communication.
 *
 * This class handles the initialization, transmission, reception, and termination 
 * of TWI (I2C) communication, including both master and slave roles. It provides 
 * methods for managing the frequency, transmission, reception of data, and 
 * callback functions for handling the received and transmitted data.
 */
class __TWI__
{
    public:
        __TWI__(volatile uint8_t* twbr, volatile uint8_t* twsr, volatile uint8_t* twar, volatile uint8_t* twdr, volatile uint8_t* twcr, volatile uint8_t* twamr);
        ~__TWI__();

        const uint8_t begin       (const uint32_t frequency);
        const uint8_t begin       (void);
        const uint8_t begin       (const uint8_t address);
        const uint8_t setFrequency(const uint32_t frequency);

        const uint8_t beginTransmission(const uint8_t address);
        const uint8_t write            (const uint8_t byte);
        const uint8_t write            (const uint8_t* bytes, const uint8_t size);
        const uint8_t write            (const void* data, const uint8_t size);
        const uint8_t endTransmission  (const uint8_t sendStop);
        const uint8_t endTransmission  (void);

        const uint8_t requestFrom(const uint8_t address, uint8_t quantity, const uint8_t sendStop);
        const uint8_t requestFrom(const uint8_t address, uint8_t quantity);
        const uint8_t available  (void);
        const uint8_t read       (void);
        const uint8_t end        (void);

        void setRxCallback(void (*function)(const uint8_t size));
        void setTxCallback(void (*function)(void));

        void isr(void);

    private:
        volatile uint8_t* twbr;   //< Pointer to the TWI bit rate register.
        volatile uint8_t* twsr;   //< Pointer to the TWI status register.
        volatile uint8_t* twar;   //< Pointer to the TWI address register.
        volatile uint8_t* twdr;   //< Pointer to the TWI data register.
        volatile uint8_t* twcr;   //< Pointer to the TWI control register.
        volatile uint8_t* twamr;  //< Pointer to the TWI address mask register.
        
        uint8_t began;                            //< Flag indicating whether TWI communication has begun.
        uint32_t frequency;                       //< The current frequency of the TWI interface.
        uint8_t role;                             //< The role of the interface (master or slave).
        volatile uint8_t state;                   //< The current state of the TWI interface.
        volatile uint8_t sendStop;                //< Flag indicating whether a stop condition should be sent.
        volatile uint8_t inRepStart;              //< Flag indicating if a repeated start condition is active.
        volatile uint8_t status;                  //< The status of the current TWI operation.
        volatile uint8_t address;                 //< The address of the TWI device.
        volatile uint8_t bufferIndex;             //< The current index in the data buffer.
        volatile uint8_t bufferSize;              //< The size of the data buffer.
        volatile uint8_t buffer[TWI_BUFFER_SIZE]; //< The buffer for storing data.

        void (*rxCallback)(const uint8_t size); //< The callback function for receiving data.
        void (*txCallback)();                   //< The callback function for transmitting data.

        void releaseBus(void); //< Releases the TWI bus.
        void stop(void);       //< Sends a stop condition to terminate TWI communication.
};


#if defined(__AVR_ATmega328__)  || \
    defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega328PB__)
    extern __TWI__ TWI0;
#endif

#if defined(__AVR_ATmega328PB__)
    extern __TWI__ TWI1;
#endif

#endif
