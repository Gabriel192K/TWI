#include "TWI.h"

/**
 * @brief Constructor for the __TWI__ class.
 * 
 * This constructor initializes the __TWI__ object with the addresses of the TWI (I2C)
 * control registers. These registers are used to configure and control the I2C peripheral
 * for communication on the bus.
 * 
 * @param twbr Pointer to the TWI Bit Rate Register (TWBR).
 * @param twsr Pointer to the TWI Status Register (TWSR).
 * @param twar Pointer to the TWI Address Register (TWAR).
 * @param twdr Pointer to the TWI Data Register (TWDR).
 * @param twcr Pointer to the TWI Control Register (TWCR).
 * @param twamr Pointer to the TWI Address Mask Register (TWAMR).
 * 
 * This constructor sets up the internal pointers to the TWI registers, allowing
 * the class to access and modify them for managing I2C operations.
 */
__TWI__::__TWI__(volatile uint8_t* twbr, volatile uint8_t* twsr, volatile uint8_t* twar, volatile uint8_t* twdr, volatile uint8_t* twcr, volatile uint8_t* twamr)
{
    this->twbr = twbr;
    this->twsr = twsr;
    this->twar = twar;
    this->twdr = twdr;
    this->twcr = twcr;
    this->twamr = twamr;
}


/**
 * @brief Destructor for the __TWI__ class.
 * 
 * This destructor resets the internal pointers to the TWI (I2C) control registers 
 * by setting them to NULL. It ensures that the TWI interface is properly cleaned up 
 * when the __TWI__ object is destroyed, preventing any unintended access to the 
 * control registers after the object goes out of scope.
 * 
 * @note The destructor does not affect the hardware registers themselves, but only 
 * the internal pointers in the class.
 */
__TWI__::~__TWI__()
{
    this->twbr = NULL;
    this->twsr = NULL;
    this->twar = NULL;
    this->twdr = NULL;
    this->twcr = NULL;
    this->twamr = NULL;
}


/**
 * @brief Initializes the TWI (I2C) interface in master mode.
 * 
 * This function configures the TWI interface to operate as a master and sets the 
 * communication frequency. It also ensures that the internal state of the TWI 
 * interface is properly initialized and ready for communication.
 * 
 * @param frequency The desired I2C communication frequency (in Hz). The default 
 *                  value is 400kHz (standard for high-speed I2C).
 * 
 * @return `1` if the initialization was successful, `0` if the TWI interface 
 *         was already initialized.
 */
const uint8_t __TWI__::begin(const uint32_t frequency)
{
    if (this->began)  /**< Check if the TWI interface has already been initialized. */
        return (0);  /**< Return 0 if already initialized. */

    this->began = 1;  /**< Mark the interface as initialized. */

    this->role = TWI_ROLE_MASTER;  /**< Set the role to master. */
    this->state = TWI_READY;  /**< Set the state to 'ready'. */
    this->sendStop = 1;  /**< Enable sending STOP after communication. */
    this->inRepStart = 0;  /**< Reset the repeated start flag. */

    this->setFrequency(frequency);  /**< Set the I2C frequency. */

    ATOMIC_BLOCK(ATOMIC_FORCEON)  /**< Begin atomic block to prevent interrupt interference. */
        *this->twcr = TWI_BEGIN;  /**< Set the control register to start TWI communication. */

    return (1);  /**< Return 1 to indicate success. */
}




/**
 * @brief Initializes the TWI (I2C) interface in master mode with the default frequency.
 * 
 * This function is a simplified version of the `begin` function, which initializes 
 * the TWI interface as a master with the default communication frequency of 400kHz 
 * (defined by `TWI_DEFAULT_FREQUENCY`). It internally calls the more general `begin` 
 * function with this default frequency.
 * 
 * @return `1` if the initialization was successful, `0` if the TWI interface 
 *         was already initialized.
 * 
 * @note This function is provided for convenience, allowing the user to initialize 
 *       the TWI interface with the default frequency without needing to specify it.
 * 
 * @see begin(uint32_t frequency) for more control over the frequency setting.
 */
const uint8_t __TWI__::begin(void)
{
    return (this->begin(TWI_DEFAULT_FREQUENCY));
}


/**
 * @brief Initializes the TWI (I2C) interface in slave mode with the specified address.
 * 
 * This function configures the TWI interface to operate in slave mode, setting the 
 * device address for communication. The address is shifted left by one bit, as 
 * required by the TWI protocol. This function also ensures that the TWI interface 
 * is not already initialized before proceeding.
 * 
 * @param address The 7-bit address to assign to this device in slave mode. The 
 *                address will be shifted left by 1 bit as required by the TWI 
 *                protocol.
 * 
 * @return `1` if the initialization was successful, `0` if the TWI interface 
 *         was already initialized.
 * 
 * @note This function sets the device to slave mode with the specified address.
 *       Ensure that the address provided is valid and that the master device 
 *       communicates with this address.
 * 
 * @see begin(uint32_t frequency) for master mode initialization, or begin() 
 *      for default frequency initialization in master mode.
 */
const uint8_t __TWI__::begin(const uint8_t address)
{
    if (this->began)  /**< Check if the TWI interface has already been initialized. */
        return (0);  /**< Return 0 if already initialized. */

    this->began = 1;  /**< Mark the interface as initialized. */

    this->role = TWI_ROLE_SLAVE;  /**< Set the role to slave. */
    this->address = address << 1;  /**< Set the slave address, shifting it by 1 bit for proper alignment. */

    ATOMIC_BLOCK(ATOMIC_FORCEON)  /**< Begin atomic block to prevent interrupt interference. */
    {
        *this->twar = this->address;  /**< Set the TWI address register to the configured address. */
        *this->twcr = TWI_BEGIN;  /**< Set the control register to start TWI communication in slave mode. */
    }

    return (1);  /**< Return 1 to indicate success. */
}



/**
 * @brief Sets the communication frequency for the TWI (I2C) interface.
 * 
 * This function configures the frequency of communication for the TWI interface. 
 * It only works if the TWI is operating in master mode. The frequency is set by 
 * adjusting the TWI Bit Rate Register (TWBR) based on the desired frequency. 
 * The calculation assumes the use of the default TWI prescaler, which is set for 
 * standard operation at a typical microcontroller clock speed.
 * 
 * @param frequency The desired TWI communication frequency (in Hz).
 * 
 * @return `1` if the frequency was successfully set, `0` if the TWI interface is 
 *         not in master mode.
 */
const uint8_t __TWI__::setFrequency(const uint32_t frequency)
{
    if (this->role != TWI_ROLE_MASTER)  /**< Check if the TWI is not in master mode. */
        return (0);  /**< Return 0 if the TWI is not in master mode. */

    this->frequency = frequency;  /**< Set the desired communication frequency. */

    *this->twbr = ((F_CPU / this->frequency) - 16) / 2;  /**< Calculate and set the Bit Rate Register for the TWI frequency. */
    
    return (1);  /**< Return 1 to indicate that the frequency was successfully set. */
}


/**
 * @brief Starts the transmission of data to a specified TWI (I2C) address.
 * 
 * This function initiates a data transmission to a specific address by first 
 * ensuring that the TWI is in master mode. It waits for the TWI to be ready, 
 * then sets up the necessary internal state for transmitting data. It also 
 * prepares the buffer for the data to be transmitted.
 * 
 * @param address The 7-bit address of the TWI slave device to communicate with.
 * 
 * @return `1` if the transmission was successfully started, `0` if the TWI 
 *         interface is not in master mode or if the transmission could not 
 *         be started.
 */
const uint8_t __TWI__::beginTransmission(const uint8_t address)
{
    if (this->role != TWI_ROLE_MASTER)  /**< Check if the TWI is not in master mode. */
        return (0);  /**< Return 0 if the TWI is not in master mode. */

    while (this->state != TWI_READY);  /**< Wait for the TWI interface to be ready for transmission. */
    
    this->state = TWI_MTX;  /**< Set the state to master transmit mode. */
    this->address = (address << 1) | TW_WRITE;  /**< Prepare the address for writing by shifting it left and setting the write bit. */
    this->bufferIndex = 0;  /**< Reset the buffer index to the beginning for storing transmitted data. */
    this->bufferSize = 0;  /**< Initialize the buffer size to zero, indicating no data yet in the buffer. */
    
    return (1);  /**< Return 1 to indicate the transmission was successfully started. */
}


/**
 * @brief Writes a byte of data to the transmission buffer.
 * 
 * This function stores a byte of data in the transmission buffer to be sent 
 * later. It ensures that the buffer does not exceed its defined size. The 
 * function will return `1` if the byte was successfully added to the buffer, 
 * and `0` if the buffer is full.
 * 
 * @param byte The byte of data to be written into the transmission buffer.
 * 
 * @return `1` if the byte was successfully written to the buffer, `0` if 
 *         the buffer is full and cannot accommodate more data.
 */
const uint8_t __TWI__::write(const uint8_t byte)
{
    if (this->bufferSize >= TWI_BUFFER_SIZE)  /**< Check if the buffer has reached its maximum size. */
        return (0);  /**< Return 0 if the buffer is full and cannot accept more data. */
    
    this->buffer[this->bufferSize++] = byte;  /**< Add the byte to the buffer and increment the buffer size. */
    
    return (1);  /**< Return 1 to indicate the byte was successfully written to the buffer. */
}


/**
 * @brief Writes a sequence of bytes to the transmission buffer.
 * 
 * This function iterates through a sequence of bytes and writes each byte 
 * to the transmission buffer. If any byte cannot be written (due to the buffer 
 * being full), the function immediately returns `0`. If all bytes are successfully 
 * written, the function returns `1`.
 * 
 * @param bytes Pointer to the array of bytes to be written into the transmission buffer.
 * @param size The number of bytes to write to the buffer.
 * 
 * @return `1` if all bytes were successfully written to the buffer, `0` if 
 *         any byte failed to be written due to the buffer being full.
 */
const uint8_t __TWI__::write(const uint8_t* bytes, const uint8_t size)
{
    for (const uint8_t* p = bytes; p < (bytes + size); p++)  /**< Loop through each byte in the input array. */
        if (!this->write(*p))  /**< Try to write the current byte to the buffer. If it fails, return 0. */
            return (0);  /**< Return 0 if any byte cannot be written due to the buffer being full. */
    
    return (1);  /**< Return 1 if all bytes were successfully written to the buffer. */
}


/**
 * @brief Writes a sequence of data to the transmission buffer.
 * 
 * This function allows writing any type of data to the transmission buffer 
 * by casting the input data pointer to a `const uint8_t*`. It then delegates 
 * the actual writing process to the `write` function that handles byte arrays.
 * If any byte cannot be written (due to the buffer being full), the function 
 * returns `0`. If all data is successfully written, it returns `1`.
 * 
 * @param data Pointer to the data to be written to the transmission buffer.
 * @param size The number of bytes of data to write.
 * 
 * @return `1` if all bytes were successfully written to the buffer, `0` if 
 *         any byte failed to be written due to the buffer being full.
 */
const uint8_t __TWI__::write(const void* data, const uint8_t size)
{
    return (this->write((const uint8_t*)data, size));  /**< Cast the data pointer to uint8_t* and call the byte-array write function. */
}


/**
 * @brief Ends a transmission by either sending a STOP or repeated START condition.
 * 
 * This function ends the transmission in the I2C protocol. If `sendStop` is set to `1`, 
 * a STOP condition will be sent to indicate the end of the communication. If the master 
 * is in a repeated start condition (due to a prior communication), the function ensures 
 * that the proper state transitions occur and prevents unnecessary retransmissions of 
 * the start condition.
 * 
 * The function waits until the current transmission is completed before returning the 
 * status. The function is valid only when the TWI role is set to master.
 * 
 * @param sendStop A flag that determines whether to send a STOP condition (`1`) or 
 *                 a repeated START condition (`0`).
 * 
 * @return `1` if the transmission ended successfully, `0` if the role is not master.
 */
const uint8_t __TWI__::endTransmission(const uint8_t sendStop)
{
    if (this->role != TWI_ROLE_MASTER)  /**< Check if the role is master; if not, return 0. */
        return (0);

    this->sendStop = sendStop;  /**< Set the sendStop flag to the provided value. */
    
    // If we're in a repeated start, we've already sent the START in the ISR. Don't do it again.
    if (this->inRepStart)  /**< Check if we are in a repeated start condition. */
    {
        // In the repeated start state, the start condition has already been sent.
        // We need to clean up before enabling interrupts.
        this->inRepStart = 0;  /**< Reset the repeated start flag. */
        *this->twdr = this->address;  /**< Write the address to the data register. */
        *this->twcr = TWI_SEND_ACK;  /**< Send an ACK to continue the transmission. */
    }
    else
        *this->twcr = TWI_SEND_START;  /**< Send the START condition. */

    while(this->state == TWI_MTX);  /**< Wait until the transmission is complete (not in master transmit mode). */
    
    return (this->status);  /**< Return the transmission status. */
}


/**
 * @brief Ends a transmission by sending a STOP condition.
 * 
 * This function ends the transmission by sending a STOP condition, which is a signal 
 * to indicate the end of the communication in the I2C protocol. It simply calls the 
 * `endTransmission` function with the `sendStop` flag set to `1` to send the STOP 
 * condition.
 * 
 * @return `1` if the transmission ended successfully, `0` if the role is not master.
 */
const uint8_t __TWI__::endTransmission(void)
{
    return (this->endTransmission((const uint8_t)1));  /**< Call the `endTransmission` with `sendStop` set to 1 to send the STOP condition. */
}


/**
 * @brief Requests data from a slave device on the I2C bus.
 * 
 * This function initiates a request to a slave device, expecting a specified number 
 * of bytes to be received. It configures the TWI interface to enter reception mode 
 * and reads the data into a buffer. The function ensures proper handling of repeated 
 * starts and ACK/NACK control during the reception.
 * 
 * @param address The I2C address of the slave device.
 * @param quantity The number of bytes to request from the slave device.
 * @param sendStop A flag to indicate whether a STOP condition should be sent after 
 *                 the request (default is `1` to send STOP, `0` to keep the bus open).
 * 
 * @return The number of bytes received or `255` if the requested quantity exceeds 
 *         the buffer size. Returns `0` if the role is not master.
 */
const uint8_t __TWI__::requestFrom(const uint8_t address, uint8_t quantity, const uint8_t sendStop)
{
    if (this->role != TWI_ROLE_MASTER)  /**< Check if the role is MASTER, return 0 if not. */
        return (0);

    if (quantity > TWI_BUFFER_SIZE)  /**< Check if requested quantity exceeds buffer size, return 255 if true. */
        return (255);

    while (this->state != TWI_READY);  /**< Wait until TWI state is ready. */
    this->state = TWI_MRX;  /**< Set state to master receiver (MRX). */
    this->sendStop = sendStop;  /**< Set the sendStop flag to determine whether to send a STOP condition. */
    this->bufferIndex = 0;  /**< Reset buffer index. */
    
    // Set buffer size to quantity - 1 because we will handle the last byte differently
    this->bufferSize = quantity - 1;  

    this->address = (address << 1) | TW_READ;  /**< Set the address for reading (shifted and added TW_READ). */

    // Handle repeated start if needed.
    if (this->inRepStart)  /**< If we are in a repeated start state... */
    {
        this->inRepStart = 0;  /**< Clear repeated start flag. */
        *this->twdr = this->address;  /**< Write the address to the TWI data register. */
        *this->twcr = TWI_SEND_ACK;  /**< Send ACK and continue. */
    }
    else
        *this->twcr = TWI_SEND_START;  /**< Send start condition for a normal request. */

    while(this->state == TWI_MRX);  /**< Wait until the data reception is completed. */

    // Adjust quantity based on the actual number of received bytes
    if (this->bufferIndex < quantity)  /**< If fewer bytes were received than requested... */
        quantity = this->bufferIndex;  /**< Adjust requested quantity to the actual received quantity. */
    
    this->bufferSize = quantity;  /**< Update buffer size with the number of received bytes. */
    this->bufferIndex = 0;  /**< Reset buffer index for next operation. */
    
    return (quantity);  /**< Return the number of bytes received. */
}


/**
 * @brief Requests data from a slave device on the I2C bus.
 * 
 * This function is a simplified version of `requestFrom` that assumes a STOP condition 
 * should be sent after the request. It is used when no specific need for handling 
 * the `sendStop` flag arises. The function forwards the request to the full 
 * `requestFrom` function with the `sendStop` parameter set to `1`.
 * 
 * @param address The I2C address of the slave device.
 * @param quantity The number of bytes to request from the slave device.
 * 
 * @return The number of bytes received, or `255` if the requested quantity exceeds 
 *         the buffer size. Returns `0` if the role is not master.
 */
const uint8_t __TWI__::requestFrom(const uint8_t address, uint8_t quantity)
{
    return (this->requestFrom(address, quantity, (const uint8_t)1));  /**< Call the full requestFrom with sendStop set to 1. */
}


/*!
 * @brief  Checking the available amount of bytes received into the buffer
 * @return Returns the available amount of bytes received into the buffer
 */
const uint8_t __TWI__::available(void)
{
    return (this->bufferSize - this->bufferIndex);
}

/*!
 * @brief  Reading a byte from the buffer
 * @return Returns 0 if the buffer index has reached the buffer size
 *         Returns the element of buffer if successful
 */
const uint8_t __TWI__::read(void)
{
    if (this->bufferIndex >= this->bufferSize)
        return (0);
    return (this->buffer[this->bufferIndex++]);
}

/*!
 * @brief  Ending the <TWI> bus communication
 * @return Returns 0 if the <TWI> bus is already stopped
 *         Returns 1 if the <TWI> bus was stopped successfully
 */
const uint8_t __TWI__::end(void)
{
    if (!this->began)
        return (0);
    this->began = 0;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        *this->twcr = TWI_END;
        *this->twar = 0;
        *this->twcr = TWI_BEGIN;
        *this->twbr = 0;
        this->role = TWI_ROLE_MASTER;
        this->address = *this->twar;
    }
        
    return (1);
}

void __TWI__::setRxCallback(void (*function)(const uint8_t))
{
    this->rxCallback = function;
}

void __TWI__::setTxCallback(void (*function)(void))
{
    this->txCallback = function;
}

void __TWI__::isr(void)
{
    this->status = *this->twsr & 0xF8;
    switch (this->status)
    {
        /* ALL MASTER */
        case TW_START:                                                        // Start condition detected
        case TW_REP_START:                                                    // Repeated start condition detected
            *this->twdr = this->address;                                      // Read the address into TWDR
            *this->twcr = TWI_SEND_ACK;                                       // Send an ACK to master
            break;
        
        /* MASTER TRANSMITTER */
        case TW_MT_SLA_ACK:                                                   // Addressed, returned ack
        case TW_MT_DATA_ACK:                                                  // Data received, returned ack
            if (this->bufferIndex < this->bufferSize)                         // If buffer index is within buffer size
            {
                *this->twdr = this->buffer[this->bufferIndex++];              // Write a byte into WTDR from buffer
                *this->twcr = TWI_SEND_ACK;                                   // Send an ACK to master
            }
            else                                                              // If no more data should be sent
            {
                if (this->sendStop)                                           // If a stop condition should be sent
                    this->stop();                                             // Send a stop condition signal
                else                                                          // If transmission should continue
                {
                    this->inRepStart = 1;                                     // We're in repeated start now
                    // don't enable the interrupt. We'll generate the start, but we
                    // avoid handling the interrupt until we're in the next transaction,
                    // at the point where we would normally issue the start.
                    *this->twcr = TWI_SEND_REP_START;                         // Send a repeated start condition signal
                    this->state = TWI_READY;                                  // Now we are ready fro more transactions
                }
            }
            break;
        case TW_MT_SLA_NACK:                                                  // Addressed, returned nack
            this->stop();                                                     // Send a stop condition
            break;
        case TW_MT_DATA_NACK:                                                 // Data received, returned nack
            this->stop();                                                     // Send a stop condition
            break;
        case TW_MT_ARB_LOST:                                                  // Arbitration lost as master
            this->releaseBus();                                               // Release the bus
            break;
            
        // MASTER RECEIVER
        case TW_MR_DATA_ACK:
            this->buffer[this->bufferIndex++] = *this->twdr;
            // No break needed
        case TW_MR_SLA_ACK:
            if(this->bufferIndex < this->bufferSize)
                *this->twcr = TWI_SEND_ACK;
            else
                *this->twcr = TWI_SEND_NACK;
            break;
        case TW_MR_DATA_NACK:
            this->buffer[this->bufferIndex++] = *this->twdr;
            if (this->sendStop)
                this->stop();
            else
            {
                this->inRepStart = 1;
                *this->twcr = TWI_SEND_REP_START;
                this->state = TWI_READY;
            } 
	        break;
        case TW_MR_SLA_NACK:
            this->stop();
            break;

        /* SLAVE RECEIVER */ 
        case TW_SR_SLA_ACK:                                      // Addressed, returned ack
        case TW_SR_GCALL_ACK:                                    // Addressed generally, returned ack
        case TW_SR_ARB_LOST_SLA_ACK:                             // Lost arbitration in slave addressing, returned ack
        case TW_SR_ARB_LOST_GCALL_ACK:                           // Lost arbitration in general call, returned ack
            this->state = TWI_SRX;                               // Set the state as slave RX
            this->bufferIndex = 0;                               // Reset the index
            *this->twcr = TWI_SEND_ACK;                          // Send an ACK to master
            break;
        case TW_SR_DATA_ACK:                                     // Data received, returned ack
        case TW_SR_GCALL_DATA_ACK:                               // Data received generally, returned ack
            if (this->bufferIndex < TWI_BUFFER_SIZE)             // If buffer index is withing buffer size bounds
            {
                this->buffer[this->bufferIndex++] = *this->twdr; // Read a byte from TWDR into buffer
                *this->twcr = TWI_SEND_ACK;                      // Send an ACK to master
            }
            else
                *this->twcr = TWI_SEND_NACK;                     // Send an NACK to master
            break;
        case TW_SR_STOP:                                         // Stop or repeated start condition received
            this->stop();                                        // Send a stop condition signal
            this->bufferSize = this->bufferIndex;                // The size is where the index stopped
            this->bufferIndex = 0;                               // Now reset the index
            if (this->rxCallback != NULL)                        // If an RX callback function was registered
                this->rxCallback(this->bufferIndex);             // Call the RX callback function
            this->releaseBus();                                  // Release the bus
            break;
        case TW_SR_DATA_NACK:                                    // Data received, returned nack
        case TW_SR_GCALL_DATA_NACK:                              // Data received generally, returned nack
            *this->twcr = TWI_SEND_NACK;                         // Send an NACK to master
            break;

        // SLAVE TRANSMITTER
        case TW_ST_SLA_ACK:          // addressed, returned ack
        case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
            this->state = TWI_STX;
            this->bufferIndex = 0;
            this->bufferSize = 0;
            if (this->txCallback != NULL)
                this->txCallback();
            
            if(!this->bufferSize)
            {
                this->bufferSize++;
                this->buffer[0] = 0xFF;
            }
            // NO NEED FOR BRAKE
        case TW_ST_DATA_ACK:
            *this->twdr = this->buffer[this->bufferIndex++];
            if (this->bufferIndex < this->bufferSize)
                *this->twcr = TWI_SEND_ACK;
            else
                *this->twcr = TWI_SEND_NACK;
            break;
        case TW_ST_DATA_NACK: // received nack, we are done 
        case TW_ST_LAST_DATA: // received ack, but we are done already!
            *this->twcr = TWI_SEND_ACK;
            this->state = TWI_READY;
            break;
        // ALL BOTH MASTER AND SLAVE
        case TW_NO_INFO:
            break;
        case TW_BUS_ERROR:
            this->stop();
            break;
    }
}

void __TWI__::releaseBus(void)
{
    *this->twcr = TWI_SEND_ACK;
    this->state = TWI_READY;
}

void __TWI__::stop(void)
{
    *this->twcr = TWI_SEND_STOP;
    while(*this->twcr & (1 << TWSTO));
    this->state = TWI_READY;
}
