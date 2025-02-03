#ifndef __TWI_H__
#define __TWI_H__

/* Dependecies */
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/atomic.h>
#include <util/delay.h>

/* Macros */
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

class __TWI__
{
    public:
        __TWI__(volatile uint8_t* twbr, volatile uint8_t* twsr, volatile uint8_t* twar, volatile uint8_t* twdr, volatile uint8_t* twcr, volatile uint8_t* twamr);
        ~__TWI__();
        const uint8_t begin            (const uint32_t frequency);
        const uint8_t begin            (void);
        const uint8_t begin            (const uint8_t address);
        const uint8_t setFrequency     (const uint32_t frequency);
        const uint8_t beginTransmission(const uint8_t address);
        const uint8_t write            (const uint8_t byte);
        const uint8_t write            (const uint8_t* bytes, const uint8_t size);
        const uint8_t write            (const void* data, const uint8_t size);
        const uint8_t endTransmission  (const uint8_t sendStop);
        const uint8_t endTransmission  (void);
        const uint8_t requestFrom      (const uint8_t address, uint8_t quantity, const uint8_t sendStop);
        const uint8_t requestFrom      (const uint8_t address, uint8_t quantity);
        const uint8_t available        (void);
        const uint8_t read             (void);
        const uint8_t end              (void);
        void          setRxCallback    (void (*function)(const uint8_t));
        void          setTxCallback    (void (*function)(void));
        void          isr              (void);
    private:
        /* General */
        volatile uint8_t* twbr;
        volatile uint8_t* twsr;
        volatile uint8_t* twar;
        volatile uint8_t* twdr;
        volatile uint8_t* twcr;
        volatile uint8_t* twamr;
        uint8_t began;
        uint32_t frequency;
        uint8_t role;
        volatile uint8_t state;
        volatile uint8_t sendStop;
        volatile uint8_t inRepStart;
        volatile uint8_t status;
        volatile uint8_t address;
        volatile uint8_t bufferIndex;
        volatile uint8_t bufferSize;
        volatile uint8_t buffer[TWI_BUFFER_SIZE];

        void (*rxCallback)(const uint8_t size);
        void (*txCallback)();

        void releaseBus(void);
        void stop(void);
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
