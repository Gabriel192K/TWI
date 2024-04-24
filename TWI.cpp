#include "TWI.h"

/*!
 * @brief  __TWI__ constructor
 * @param  TWBR, TWSR, TWAR, TWDR, TWCR, TWAMR
 *         The TWI registers
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

/*!
 * @brief  __TWI__ Destructor
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

/*!
 * @brief  Begins the TWI implementation using a fixed frequency
 * @param  frequency
 *         The frequency of TWI bus communication
 */
void __TWI__::begin(const uint32_t frequency)
{
    if (this->started)
        return;
    this->started = 1;

    this->role = TWI_ROLE_MASTER;
    this->state = TWI_READY;
    this->sendStop = 1;
    this->inRepStart = 0;

    // Use internall pullup of <SDA> and <SCL>
    // Not needed if external pullup's are inserted into the circuit
    DDRC = DDRC & ~((1 << 4) | (1 << 5));  // Set <DDR> as <INPUT>
    PORTC = PORTC | ((1 << 4) | (1 << 5)); // Set <DOR> as <HIGH>

    this->setFrequency(frequency);
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
    }
}

/*!
 * @brief  Setting the TWI bus communication frequency
 * @param  frequency
 *         The frequency of TWI bus communication
 * @return Returns 0 if the role is not <MASTER>, otherwise returns a 1
 */
const uint8_t __TWI__::setFrequency(const uint32_t frequency)
{
    if (this->role != TWI_ROLE_MASTER)
        return (0);
    this->frequency = frequency;
    *this->twbr = ((F_CPU / this->frequency) - 16) / 2;
    return (1);
}

/*!
 * @brief  Begining a transmission as <MASTER> to a fixed <SLAVE> address
 * @param  address
 *         The address of TWI bus <SLAVE>
 * @return Returns 0 if the role is not <MASTER>, otherwise returns a 1
 */
const uint8_t __TWI__::beginTransmission(const uint8_t address)
{
    if (this->role != TWI_ROLE_MASTER)
        return (0);

    while (this->state != TWI_READY);
    this->state = TWI_MTX;
    this->address = (address << 1) | TW_WRITE;
    this->bufferIndex = 0;
    this->bufferSize = 0;
    return (1);
}

/*!
 * @brief  Writing a byte into the buffer
 * @param  byte
 *         The byte to be written into the buffer
 * @return Returns 0 if the byte doesnt fit into the buffer, otherwise returns a 1
 */
const uint8_t __TWI__::write(const uint8_t byte)
{
    if ((this->bufferSize + 1) >= TWI_BUFFER_SIZE)
        return (0);
    this->buffer[this->bufferSize++] = byte;
    return (1);
}

/*!
 * @brief  Writing an array of bytes into the buffer that has a known size
 * @param  bytes
 *         The array of bytes to be written into the buffer
 * @param  size
 *         The size of the array of bytes to be written into the buffer
 * @return Returns 0 if one of the bytes from the array of bytes don't fit into the buffer anymore, otheriwse returns a 1
 */
const uint8_t __TWI__::write(const uint8_t* bytes, const uint8_t size)
{
    for (const uint8_t* p = bytes; p < (bytes + size); p++)
        if (!this->write(*p))
            return (0);
    return (1);
}

/*!
 * @brief  Writing any type of data into the buffer that has a known size
 * @param  bytes
 *         The data to be written into the buffer
 * @param  size
 *         The size of the data to be written into the buffer
 * @return Returns 0 if one of the bytes from the data don't fit into the buffer anymore, otheriwse returns a 1
 */
const uint8_t __TWI__::write(const void* data, const uint8_t size)
{
    return (this->write((const uint8_t*)data, size));
}

/*!
 * @brief  Ending the transmission as <MASTER>
 * @param  sendStop
 *         The condition for sending or not sending a stop signal over the TWI bus at the end of transmission
 * @return Returns 0 if the role is not <MASTER>
 *         Returns 1 if successfull
 *         Returns 2 if <SLAVE> did not <ACK> the address
 *         Returns 3 if <SLAVE> did not <ACK> the data
 *         Returns 4 if an unknown error happened
 */
const uint8_t __TWI__::endTransmission(const uint8_t sendStop)
{
    if (this->role != TWI_ROLE_MASTER)
        return (0);

    this->sendStop = sendStop;
    this->error = 0xFF;
    // if we're in a repeated start, then we've already sent the START
    // in the ISR. Don't do it again.
    //
    if (this->inRepStart)
    {
        // if we're in the repeated start state, then we've already sent the start,
        // (@@@ we hope), and the TWI statemachine is just waiting for the address byte.
        // We need to remove ourselves from the repeated start state before we enable interrupts,
        // since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
        // up. Also, don't enable the START interrupt. There may be one pending from the 
        // repeated start that we sent outselves, and that would really confuse things.
        this->inRepStart = 0; // remember, we're dealing with an ASYNC ISR
        *this->twdr =  this->address;				
        *this->twcr = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
    }
    else
        *this->twcr = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);

    while(this->state == TWI_MTX);
    
    if (this->error == 0xFF)
        return (1);
    else if (this->error == TW_MT_SLA_NACK)
        return (2);
    else if (this->error == TW_MT_DATA_NACK)
        return (3);
    return (4);
}

/*!
 * @brief  Ending the transmission as <MASTER> with a stop condition
 * @return Returns 0 if the role is not <MASTER>
 *         Returns 1 if successfull
 *         Returns 2 if <SLAVE> did not <ACK> the address
 *         Returns 3 if <SLAVE> did not <ACK> the data
 *         Returns 4 if an unknown error happened
 */
const uint8_t __TWI__::endTransmission(void)
{
    return (this->endTransmission((const uint8_t)1));
}

/*!
 * @brief  Requesting data as a <MASTER>
 * @param  address
 *         The address of TWI bus <SLAVE>
 * @param  quantity
 *         The quantity in bytes requested
 * @param  sendStop
 *         The condition for sending or not sending a stop signal over the TWI bus at the end of request
 * @return Returns 0 if the role is not <MASTER>
 *         Returns 255 if the quantity requested don't fit into the buffer
 *         Returns quantity if successfull
 */
const uint8_t __TWI__::requestFrom(const uint8_t address, uint8_t quantity, const uint8_t sendStop)
{
    if (this->role != TWI_ROLE_MASTER)
        return (0);

    if (quantity > TWI_BUFFER_SIZE)
        return (255);
    
    while (this->state != TWI_READY);
    this->state = TWI_MRX;
    this->sendStop = sendStop;
    this->error = 0xFF;
    this->bufferIndex = 0;
    // This is not intuitive, read on...
    // On receive, the previously configured ACK/NACK setting is transmitted in
    // response to the received byte before the interrupt is signalled. 
    // Therefor we must actually set NACK when the _next_ to last byte is
    // received, causing that NACK to be sent in response to receiving the last
    // expected byte of data.
    this->bufferSize = quantity - 1;
    this->address = (address << 1) | TW_READ;
    // if we're in a repeated start, then we've already sent the START
    // in the ISR. Don't do it again.
    //
    if (this->inRepStart)
    {
        // if we're in the repeated start state, then we've already sent the start,
        // (@@@ we hope), and the TWI statemachine is just waiting for the address byte.
        // We need to remove ourselves from the repeated start state before we enable interrupts,
        // since the ISR is ASYNC, and we could get confused if we hit the ISR before cleaning
        // up. Also, don't enable the START interrupt. There may be one pending from the 
        // repeated start that we sent outselves, and that would really confuse things.
        this->inRepStart = 0; // remember, we're dealing with an ASYNC ISR
        *this->twdr =  this->address;				
        *this->twcr = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE);
    }
    else
        *this->twcr = _BV(TWINT) | _BV(TWEA) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);

    while(this->state == TWI_MRX);

    if (this->bufferIndex < quantity)
        quantity = this->bufferIndex;
    
    this->bufferSize = quantity;
    this->bufferIndex = 0;
    
    return (quantity);
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
    if (!this->started)
        return (0);
    this->started = 0;

    DDRC = DDRC & ~((1 << 4) | (1 << 5));
    PORTC = PORTC & ~((1 << 4) | (1 << 5));
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        *this->twcr = 0x00;
    }
    return (1);
}

// Work in progress for TWI SLAVE
void __TWI__::begin(const uint8_t address)
{
    if (this->started)
        return;
    this->started = 1;

    this->role = TWI_ROLE_SLAVE;
    this->address = address << 1;
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        *this->twar = this->address;
        *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
    }
}

void __TWI__::setRxCallback(void (*function)(uint8_t*, const uint8_t))
{
    this->rxCallback = function;
}

void __TWI__::setTxCallback(void (*function)(void))
{
    this->txCallback = function;
}

// ISR to be modified for TWI SLAVE
void __TWI__::isr(void)
{
    switch (*this->twsr & 0xF8)
    {
        // ALL MASTER
        case TW_START:
        case TW_REP_START:
            *this->twdr = this->address;
            *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            break;
        // MASTER TRANSMITTER
        case TW_MT_SLA_ACK:
        case TW_MT_DATA_ACK:
            if (this->bufferIndex < this->bufferSize)
            {
                *this->twdr = this->buffer[this->bufferIndex++];
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            }
            else
            {
                if (this->sendStop)
                    this->stop();
                else
                {
                    this->inRepStart = 1;
                    *this->twcr = _BV(TWINT) | _BV(TWSTA)| _BV(TWEN);
                    this->state = TWI_READY;
                }
            }
            break;
        case TW_MT_SLA_NACK:
            this->error = TW_MT_SLA_NACK;
            this->stop();
            break;
        case TW_MT_DATA_NACK:
            this->error = TW_MT_DATA_NACK;
            this->stop();
            break;
        case TW_MT_ARB_LOST:
            this->error = TW_MT_ARB_LOST;
            this->releaseBus();
            break;
        // MASTER RECEIVER
        case TW_MR_DATA_ACK:
            this->buffer[this->bufferIndex++] = TWDR;
            // No break needed
        case TW_MR_SLA_ACK:
            if(this->bufferIndex < this->bufferSize)
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            else
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
            break;
        case TW_MR_DATA_NACK:
            this->buffer[this->bufferIndex++] = TWDR;
            if (this->sendStop)
                this->stop();
            else
            {
                this->inRepStart = 1;
                *this->twcr = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
                this->state = TWI_READY;
            } 
	        break;
        case TW_MR_SLA_NACK:
            this->stop();
            break;
        // SLAVE RECEIVER
        case TW_SR_SLA_ACK:   // addressed, returned ack
        case TW_SR_GCALL_ACK: // addressed generally, returned ack
        case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
        case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
            this->state = TWI_SRX;
            this->slaveRxBufferIndex = 0;
            *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            break;
        case TW_SR_DATA_ACK:       // data received, returned ack
        case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
            if (this->slaveRxBufferIndex < TWI_BUFFER_SIZE)
            {
                this->slaveRxBuffer[this->slaveRxBufferIndex++] = *this->twdr;
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            }
            else
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
            break;
        case TW_SR_STOP: // stop or repeated start condition received
            this->stop();
            if (this->rxCallback != NULL)
                this->rxCallback((uint8_t*)this->slaveRxBuffer, this->slaveRxBufferIndex);
            this->releaseBus();
            break;
            PORTD ^= (1 << 2);
        case TW_SR_DATA_NACK:
        case TW_SR_GCALL_DATA_NACK:
            *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
            break;
        // SLAVE TRANSMITTER
        case TW_ST_SLA_ACK:          // addressed, returned ack
        case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
            this->state = TWI_STX;
            this->slaveTxBufferIndex = 0;
            this->slaveTxBufferSize = 0;
            if (this->txCallback != NULL)
                this->txCallback();
            
            if(!this->slaveTxBufferSize)
            {
                this->slaveTxBufferSize++;
                this->slaveTxBuffer[0] = 0xFF;
            }
            // NO NEED FOR BRAKE
        case TW_ST_DATA_ACK:
            *this->twdr = this->slaveTxBuffer[this->slaveTxBufferIndex++];
            if (this->slaveTxBufferIndex < this->slaveTxBufferSize)
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            else
                *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
            break;
        case TW_ST_DATA_NACK: // received nack, we are done 
        case TW_ST_LAST_DATA: // received ack, but we are done already!
            *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
            this->state = TWI_READY;
            break;
        // ALL BOTH MASTER AND SLAVE
        case TW_NO_INFO:
            this->error = TW_NO_INFO;
            break;
        case TW_BUS_ERROR:
            this->error = TW_BUS_ERROR;
            this->stop();
            break;
    }
}

void __TWI__::releaseBus(void)
{
    *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);
    this->state = TWI_READY;
}

void __TWI__::stop(void)
{
    *this->twcr = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);
    while(*this->twcr & _BV(TWSTO));
    this->state = TWI_READY;
}
