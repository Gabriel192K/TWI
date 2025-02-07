#ifndef __TWI_H__
#define __TWI_H__

/* Dependecies */
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/atomic.h>
#include <util/delay.h>

/**
 * @def TWI_DEFAULT_FREQUENCY
 * @brief The default frequency for I2C communication (400 kHz).
 * 
 * This macro sets the default I2C clock frequency to 400 kHz, which is commonly used
 * for high-speed I2C communication. It can be used in the configuration of the TWI (I2C) bus.
 */
#define TWI_DEFAULT_FREQUENCY (const uint32_t)400000

/**
 * @def TWI_ROLE_MASTER
 * @brief Defines the role of the device as an I2C master.
 * 
 * This macro is used to define the role of the I2C device as a master, responsible for
 * initiating and controlling communication on the bus.
 */
#define TWI_ROLE_MASTER (const uint8_t)0

/**
 * @def TWI_ROLE_SLAVE
 * @brief Defines the role of the device as an I2C slave.
 * 
 * This macro is used to define the role of the I2C device as a slave, which responds
 * to requests initiated by the master device.
 */
#define TWI_ROLE_SLAVE (const uint8_t)1

/**
 * @def TWI_READY
 * @brief The state of the I2C bus when it is ready for communication.
 * 
 * This macro defines the state when the I2C bus is in a ready state to either transmit or
 * receive data.
 */
#define TWI_READY (const uint8_t)0

/**
 * @def TWI_MRX
 * @brief The state of the I2C bus when it is receiving data as a master.
 * 
 * This macro defines the state when the I2C master is in receive mode (master receive mode).
 */
#define TWI_MRX (const uint8_t)1

/**
 * @def TWI_MTX
 * @brief The state of the I2C bus when it is transmitting data as a master.
 * 
 * This macro defines the state when the I2C master is in transmit mode (master transmit mode).
 */
#define TWI_MTX (const uint8_t)2

/**
 * @def TWI_SRX
 * @brief The state of the I2C bus when it is receiving data as a slave.
 * 
 * This macro defines the state when the I2C slave is in receive mode (slave receive mode).
 */
#define TWI_SRX (const uint8_t)3

/**
 * @def TWI_STX
 * @brief The state of the I2C bus when it is transmitting data as a slave.
 * 
 * This macro defines the state when the I2C slave is in transmit mode (slave transmit mode).
 */
#define TWI_STX (const uint8_t)4

/**
 * @def TWI_BUFFER_SIZE
 * @brief The buffer size for I2C data transmission and reception.
 * 
 * This macro defines the buffer size to be used for storing data during I2C communication,
 * with a typical value of 32 bytes.
 */
#define TWI_BUFFER_SIZE (const uint8_t)32

/**
 * @def TWI_BEGIN
 * @brief Command to initiate TWI (I2C) communication.
 * 
 * This macro defines the bit mask for enabling TWI (I2C) and interrupts, as well as enabling
 * acknowledgement after each byte.
 */
#define TWI_BEGIN ((1 << TWEN) | (1 << TWIE) | (1 << TWEA))

/**
 * @def TWI_SEND_ACK
 * @brief Command to send an acknowledgment after receiving a byte.
 * 
 * This macro sets the necessary bits to send an acknowledgment (ACK) after receiving a byte
 * in I2C communication.
 */
#define TWI_SEND_ACK ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA))

/**
 * @def TWI_SEND_NACK
 * @brief Command to send a negative acknowledgment after receiving a byte.
 * 
 * This macro sets the necessary bits to send a negative acknowledgment (NACK) after receiving
 * a byte in I2C communication.
 */
#define TWI_SEND_NACK ((1 << TWEN) | (1 << TWIE) | (1 << TWINT))

/**
 * @def TWI_SEND_START
 * @brief Command to send a START condition in I2C communication.
 * 
 * This macro sets the necessary bits to initiate an I2C communication with a START condition,
 * which signals the beginning of a new I2C transmission.
 */
#define TWI_SEND_START ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA) | (1 << TWSTA))

/**
 * @def TWI_SEND_REP_START
 * @brief Command to send a repeated START condition in I2C communication.
 * 
 * This macro sets the necessary bits to initiate an I2C communication with a repeated START condition,
 * allowing multiple I2C transactions without releasing the bus.
 */
#define TWI_SEND_REP_START ((1 << TWEN) | (1 << TWINT) | (1 << TWSTA))

/**
 * @def TWI_SEND_STOP
 * @brief Command to send a STOP condition in I2C communication.
 * 
 * This macro sets the necessary bits to end an I2C communication with a STOP condition,
 * signaling the end of the transmission.
 */
#define TWI_SEND_STOP ((1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA) | (1 << TWSTO))

/**
 * @def TWI_END
 * @brief Command to disable TWI (I2C) communication.
 * 
 * This macro defines the value used to disable TWI (I2C) communication, effectively ending
 * the communication session.
 */
#define TWI_END (const uint8_t)0


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
