#ifndef __TWI_H__
#define __TWI_H__

/* Dependecies */
#include "Core\Core.h"
#include "Time\Time.h"

/* Macros */
#define TWI_DEFAULT_FREQUENCY ((uint32_t)400000) // 400 kHz
#define TWI_DEFAULT_TIMEOUT   ((uint32_t)2)      // 2 mS

class __TWI__
{
    public:
        __TWI__(volatile uint8_t* twbr, volatile uint8_t* twsr, volatile uint8_t* twar,\
                volatile uint8_t* twdr, volatile uint8_t* twcr, volatile uint8_t* twamr);
        void    begin             (void);
        void    reset             (void);
        void    setFrequency      (uint32_t frequency);
        uint8_t beginTransmission (uint8_t address);
        uint8_t write             (uint8_t data);
        uint8_t requestFrom       (uint8_t address, uint8_t bytes);
        uint8_t read              (void);
        void    endTransmission   (void);
        void    end               (void);
    private:
        time_t timestamp;
        uint8_t requestedBytes;
        volatile uint8_t* twbr;
        volatile uint8_t* twsr;
        volatile uint8_t* twar;
        volatile uint8_t* twdr;
        volatile uint8_t* twcr;
        volatile uint8_t* twamr;
};

#if defined(__AVR_ATmega328P__)
#define SCL_DDR DDRC /* <Serial Clock> Data Direction Register */
#define SCL_DOR DORC /* <Serial Clock> Data Output Register */
#define SCL_DIR DIRC /* <Serial Clock> Data Input Register */
#define SCL_BIT 5    /* <Serial Clock> Bit */
#define SDA_DDR DDRC /* <Serial Data> Data Direction Register */
#define SDA_DOR DORC /* <Serial Data> Data Output Register */
#define SDA_DIR DIRC /* <Serial Data> Data Input Register */
#define SDA_BIT 4    /* <Serial Data> Bit */
extern __TWI__ TWI;
#else
#error "No TWI bus implementation"
#endif

#endif
