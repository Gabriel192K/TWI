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
        /**
         * @brief Constructor for the TWI class.
         * 
         * Initializes the TWI interface with the provided register addresses.
         * 
         * @param twbr Pointer to the TWI bit rate register.
         * @param twsr Pointer to the TWI status register.
         * @param twar Pointer to the TWI address register.
         * @param twdr Pointer to the TWI data register.
         * @param twcr Pointer to the TWI control register.
         * @param twamr Pointer to the TWI address mask register.
         */
        __TWI__(volatile uint8_t* twbr, volatile uint8_t* twsr, volatile uint8_t* twar, volatile uint8_t* twdr, volatile uint8_t* twcr, volatile uint8_t* twamr);

        /**
         * @brief Destructor for the TWI class.
         * 
         * Cleans up any resources used by the TWI interface.
         */
        ~__TWI__();

        /**
         * @brief Initializes the TWI interface with a specified frequency.
         * 
         * This method configures the TWI interface to operate at the given frequency.
         * 
         * @param frequency The desired frequency of the TWI interface.
         * @return uint8_t The status of the operation.
         */
        const uint8_t begin(const uint32_t frequency);

        /**
         * @brief Initializes the TWI interface with default settings.
         * 
         * This method initializes the TWI interface with default parameters.
         * 
         * @return uint8_t The status of the operation.
         */
        const uint8_t begin(void);

        /**
         * @brief Initializes the TWI interface with a specified address.
         * 
         * This method initializes the TWI interface and sets the address for the 
         * communication.
         * 
         * @param address The address to be set for the TWI interface.
         * @return uint8_t The status of the operation.
         */
        const uint8_t begin(const uint8_t address);

        /**
         * @brief Sets the TWI frequency.
         * 
         * This method adjusts the TWI interface to operate at a new frequency.
         * 
         * @param frequency The desired frequency for the TWI interface.
         * @return uint8_t The status of the operation.
         */
        const uint8_t setFrequency(const uint32_t frequency);

        /**
         * @brief Begins a transmission to a specified address.
         * 
         * This method prepares the TWI interface for transmitting data to a specific address.
         * 
         * @param address The address to send data to.
         * @return uint8_t The status of the operation.
         */
        const uint8_t beginTransmission(const uint8_t address);

        /**
         * @brief Writes a single byte of data to the TWI bus.
         * 
         * This method writes one byte to the TWI bus.
         * 
         * @param byte The byte to be written.
         * @return uint8_t The status of the operation.
         */
        const uint8_t write(const uint8_t byte);

        /**
         * @brief Writes an array of bytes to the TWI bus.
         * 
         * This method writes multiple bytes to the TWI bus.
         * 
         * @param bytes Pointer to the array of bytes to be written.
         * @param size The number of bytes to write.
         * @return uint8_t The status of the operation.
         */
        const uint8_t write(const uint8_t* bytes, const uint8_t size);

        /**
         * @brief Writes a block of data to the TWI bus.
         * 
         * This method writes a block of data to the TWI bus.
         * 
         * @param data Pointer to the data to be written.
         * @param size The size of the data block.
         * @return uint8_t The status of the operation.
         */
        const uint8_t write(const void* data, const uint8_t size);

        /**
         * @brief Ends a transmission to the TWI bus with a stop condition.
         * 
         * This method sends a stop condition at the end of a transmission.
         * 
         * @param sendStop Indicates whether a stop condition should be sent.
         * @return uint8_t The status of the operation.
         */
        const uint8_t endTransmission(const uint8_t sendStop);

        /**
         * @brief Ends a transmission to the TWI bus without a stop condition.
         * 
         * This method ends the transmission but does not send a stop condition.
         * 
         * @return uint8_t The status of the operation.
         */
        const uint8_t endTransmission(void);

        /**
         * @brief Requests data from a specified address.
         * 
         * This method requests a specified amount of data from a given address.
         * 
         * @param address The address to request data from.
         * @param quantity The number of bytes to request.
         * @param sendStop Indicates whether a stop condition should be sent after the request.
         * @return uint8_t The status of the operation.
         */
        const uint8_t requestFrom(const uint8_t address, uint8_t quantity, const uint8_t sendStop);

        /**
         * @brief Requests data from a specified address.
         * 
         * This method requests a specified amount of data from a given address without 
         * sending a stop condition.
         * 
         * @param address The address to request data from.
         * @param quantity The number of bytes to request.
         * @return uint8_t The status of the operation.
         */
        const uint8_t requestFrom(const uint8_t address, uint8_t quantity);

        /**
         * @brief Checks if data is available for reading.
         * 
         * This method checks if there is data available in the buffer to read.
         * 
         * @return uint8_t The number of available bytes.
         */
        const uint8_t available(void);

        /**
         * @brief Reads a byte of data from the TWI bus.
         * 
         * This method reads one byte of data from the TWI bus.
         * 
         * @return uint8_t The byte of data read.
         */
        const uint8_t read(void);

        /**
         * @brief Ends the TWI communication and releases the bus.
         * 
         * This method sends a stop condition and releases the TWI bus.
         * 
         * @return uint8_t The status of the operation.
         */
        const uint8_t end(void);

        /**
         * @brief Sets a callback function for receiving data.
         * 
         * This method sets a callback function to be invoked when data is received.
         * 
         * @param function The callback function to be invoked on receiving data.
         */
        void setRxCallback(void (*function)(const uint8_t));

        /**
         * @brief Sets a callback function for transmitting data.
         * 
         * This method sets a callback function to be invoked when data is transmitted.
         * 
         * @param function The callback function to be invoked on transmitting data.
         */
        void setTxCallback(void (*function)(void));

        /**
         * @brief Interrupt service routine for handling TWI communication.
         * 
         * This method is triggered when an interrupt occurs in the TWI system. It handles 
         * the communication states and manages the transitions between master and slave roles.
         */
        void isr(void);

    private:
        /* General TWI registers and state information */
        volatile uint8_t* twbr;   //*< Pointer to the TWI bit rate register.
        volatile uint8_t* twsr;   //*< Pointer to the TWI status register.
        volatile uint8_t* twar;   //*< Pointer to the TWI address register.
        volatile uint8_t* twdr;   //*< Pointer to the TWI data register.
        volatile uint8_t* twcr;   //*< Pointer to the TWI control register.
        volatile uint8_t* twamr;  //*< Pointer to the TWI address mask register.
        
        uint8_t began;            //*< Flag indicating whether TWI communication has begun.
        uint32_t frequency;       //*< The current frequency of the TWI interface.
        uint8_t role;             //*< The role of the interface (master or slave).
        volatile uint8_t state;   //*< The current state of the TWI interface.
        volatile uint8_t sendStop;//*< Flag indicating whether a stop condition should be sent.
        volatile uint8_t inRepStart;  //*< Flag indicating if a repeated start condition is active.
        volatile uint8_t status;  //*< The status of the current TWI operation.
        volatile uint8_t address; //*< The address of the TWI device.
        volatile uint8_t bufferIndex;  //*< The current index in the data buffer.
        volatile uint8_t bufferSize;   //*< The size of the data buffer.
        volatile uint8_t buffer[TWI_BUFFER_SIZE]; //*< The buffer for storing data.

        void (*rxCallback)(const uint8_t size); //*< The callback function for receiving data.
        void (*txCallback)();  //*< The callback function for transmitting data.

        void releaseBus(void);  //*< Releases the TWI bus.
        void stop(void);        //*< Sends a stop condition to terminate TWI communication.
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
