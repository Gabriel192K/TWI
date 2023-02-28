#include "TWI.h"

/* Macros */
#define TWI_BEGIN            *this->twcr = (1 << TWEN) | (1 << TWEA)
#define TWI_START            *this->twcr = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA)
#define TWI_STOP             *this->twcr = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO)
#define TWI_WRITE            *this->twcr = (1 << TWEN) | (1 << TWINT)
#define TWI_READ(ACK)        *this->twcr = (1 << TWEN) | (ACK << TWEA) | (1 << TWINT)
#define TWI_END              *this->twcr = 0
#define TWI_STATUS           (*this->twsr & 0xF8)
#define TWI_MASTER_SLAVE_ACK 0x18

/*********************************************
Function: __TWI__()
Purpose:  Constructor to __TWI__ class
Input:    None
Return:   None
*********************************************/
__TWI__::__TWI__(volatile uint8_t* twbr, volatile uint8_t* twsr, volatile uint8_t* twar,\
                 volatile uint8_t* twdr, volatile uint8_t* twcr, volatile uint8_t* twamr)
{
    this->twbr = twbr;
    this->twsr = twsr;
    this->twar = twar;
    this->twdr = twdr;
    this->twcr = twcr;
    this->twamr = twamr;
}

/*********************************************
Function: begin()
Purpose:  Start TWI bus
Input:    None
Return:   None
*********************************************/
void __TWI__::begin(void)
{
    pinMode(&SCL_DDR, SCL_BIT, INPUT);     /* Set <SCL> Data Direction Register as <INPUT> */ 
    pinMode(&SDA_DDR, SDA_BIT, INPUT);     /* Set <SDA> Data Direction Register as <INPUT>*/ 
    digitalWrite(&SCL_DOR, SCL_BIT, HIGH); /* Set <SCL> Data Output Register as <HIGH>*/
    digitalWrite(&SDA_DOR, SDA_BIT, HIGH); /* Set <SDA> Data Output Register as <HIGH>*/

    /*
     * External resistors for both SCL & SDA are required
     * Usually 4K7 or 10K are used
     */

    Time.begin();                              /* Start Time implementation  */
    this->setFrequency(TWI_DEFAULT_FREQUENCY); /* Set default <TWI> frequency */ 
    TWI_BEGIN;                                 /* Send <BEGIN> condition */
}

/*********************************************
Function: reset()
Purpose:  Reset TWI bus
Input:    None
Return:   None
*********************************************/
void __TWI__::reset(void)
{
    uint8_t twbr = *this->twbr; /* Copy current TWBR */ 
    uint8_t twar = *this->twar; /* Copy current TWAR */
    __TWI__::end();             /* Stop TWI bus */
    __TWI__::begin();           /* Start TWI bus */
    *this->twbr = twbr;         /* Write previous TWBR */ 
    *this->twar = twar;         /* Write previous TWAR */ 
}

/*********************************************
Function: setFrequency()
Purpose:  Set desired frequency of TWI bus
Input:    Frequency
Return:   None
*********************************************/
void __TWI__::setFrequency(uint32_t frequency)
{
    *this->twbr = (((F_CPU / frequency) - 16) / 2); /* Calculate <TWI> baudrate in terms of frequency */
}

/*********************************************
Function: beginTransmission()
Purpose:  Begin transmission of data
Input:    Address where data is transmitted
Return:   Status of transmission
*********************************************/
uint8_t __TWI__::beginTransmission(uint8_t address)
{
    TWI_START;                                                 /* Send <START> condition */
    timestamp = Time.millis();                                 /* Get a timestamp */ 
    while (!(*this->twcr & (1 << TWINT)))                      /* Wait for <TWINT> bit to set */ 
    {
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                     /* Reset <TWI> bus */
            return (0);
        }
    }

    address = (address << 1);                                  /* Shift address to left 1 bit */
    timestamp = Time.millis();                                 /* Get a timestamp */ 
    do
    {
        *this->twdr = address;                                 /* Write address into <TWDR> */ 
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */
        {
            this->reset();                                     /* Reset <TWI> bus */
            return (0);
        }
    } while (*this->twcr & (1 << TWWC));                       /* Wait for <TWWC> bit to clear */

    TWI_WRITE;                                                 /* Send <WRITE> condition */
    timestamp = Time.millis();                                 /* Get a timestamp */
    while (!(*this->twcr & (1 << TWINT)))                      /* Wait for <TWINT> bit to set */
    {
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                  /* Reset <TWI> bus */
            return (0);
        }
    }
    if (TWI_STATUS == TWI_MASTER_SLAVE_ACK) return (1);       /* If slave returned <ACK> */
    return (0);
}

/*********************************************
Function: write()
Purpose:  Write data on the TWI bus
Input:    Byte of data to be sent
Return:   Status of transmission
*********************************************/
uint8_t __TWI__::write(uint8_t data)
{
    *this->twdr = data;                                        /* Write data into <TWDR> */ 
    TWI_WRITE;                                                 /* Send <WRITE> condition */ 
    timestamp = Time.millis();                                 /* Get a timestamp */
    while (!(*this->twcr & (1 << TWINT)))                      /* Wait for <TWINT> bit to set */
    {
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                     /* Reset <TWI> bus */
            return (0);
        }
    }
	return (1);
}

/*********************************************
Function: requestFrom()
Purpose:  Request data from slave
Input:    Address of the slave and amount of bytes supposed to get
Return:   Amount of bytes supposed to get
*********************************************/
uint8_t __TWI__::requestFrom(uint8_t address, uint8_t bytes)
{
    requestedBytes = bytes;                                    /* Keep amount of bytes to be read */
    TWI_START;                                                 /* Send <START> condition */
    timestamp = Time.millis();                                 /* Get a timestamp */
    while (!(*this->twcr & (1 << TWINT)))                      /* Wait for <TWINT> bit to set */
    {
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                     /* Reset <TWI> bus */
            return (0);
        }
    }

    address = ((address << 1) | 1);                            /* Shift address to left 1 bit then set its least significant bit */
    timestamp = Time.millis();                                 /* Get a timestamp */
    do
    {
        TWDR = address;                                        /* Write address into <TWDR> */
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                     /* Reset <TWI> bus */
            return (0);
        }
    } while(*this->twcr & (1 << TWWC));                        /* Wait for <TWWC> bit to clear */

    TWI_WRITE;                                                 /* Send <WRITE> condition */
    timestamp = Time.millis();                                 /* Get a timestamp */
    while (!(*this->twcr & (1 << TWINT)))                      /* Wait for <TWINT> bit to set */
    {
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                     /* Reset <TWI> bus */
            return (0);
        }
    }
    return (requestedBytes);                                   /* Return amount of bytes meant to be received */
}

/*********************************************
Function: TWI_read()
Purpose:  Read data from slave
Input:    None
Return:   Data from slave
*********************************************/
uint8_t __TWI__::read(void)
{
    (--requestedBytes > 0) ? TWI_READ(1) : ((requestedBytes == 0) ? TWI_READ(0) : 0); /* If amount of bytes are over 0 send <READ> condtion with <ACK> and the last byte send an <NACK> */
    timestamp = Time.millis();                                                        /* Get a timestamp */
    while (!(*this->twcr & (1 << TWINT)))                                             /* Wait for <TWINT> bit to set */
    {
        if ((Time.millis() - timestamp) > TWI_DEFAULT_TIMEOUT)                        /* If timeout occured */ 
        {
            this->reset();                                                            /* Reset <TWI> bus */
            return (0);
        }
    }
    return *this->twdr;                                                               /* Return data stored into <TWDR> */
}

/*********************************************
Function: TWI_endTransmission()
Purpose:  End transmission of data
Input:    None
Return:   None
*********************************************/
void __TWI__::endTransmission(void)
{
    TWI_STOP;                                                /* Send <STOP> condition */
    timestamp = Time.millis();                               /* Get a timestamp  */
    while (*this->twcr & (1 << TWSTO))                       /* Wait for <STOP> bit to clear */
    {
        if (Time.millis() - timestamp > TWI_DEFAULT_TIMEOUT) /* If timeout occured */ 
        {
            this->reset();                                   /* Reset <TWI> bus */
            return;
        }  
    }
}

/*********************************************
Function: end()
Purpose:  Stop TWI bus
Input:    None
Return:   None
*********************************************/
void __TWI__::end(void)
{
    TWI_END;                              /* Send <END> condition */
    digitalWrite(&SCL_DOR, SCL_BIT, LOW); /* Set <SCL> Data Output Register as <LOW>*/
    digitalWrite(&SDA_DOR, SDA_BIT, LOW); /* Set <SDA> Data Output Register as <LOW>*/
}

#if defined(__AVR_ATmega328P__)
__TWI__ TWI = __TWI__(&TWBR, &TWSR, &TWAR, &TWDR, &TWCR, &TWAMR);
#endif
