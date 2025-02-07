/* Dependencies */
#include "TWI.h"

/**
 * @brief Initializes the TWI0 interface for different microcontroller variants.
 * 
 * This code section initializes the TWI (Two-Wire Interface) for specific AVR microcontrollers 
 * such as ATmega328, ATmega328P, and ATmega328PB. It sets up the corresponding registers 
 * and assigns an interrupt service routine (ISR) to handle the TWI interrupts.
 */
#if defined(__AVR_ATmega328__)  || \
	defined(__AVR_ATmega328P__)
    /**
     * @brief Create an instance of the TWI interface for ATmega328/328P.
     * 
     * This instantiates a __TWI__ object using the corresponding register pointers for 
     * ATmega328/328P.
     */
    __TWI__ TWI0 = __TWI__(&TWBR, &TWSR, &TWAR, &TWDR, &TWCR, &TWAMR);

    /**
     * @brief TWI interrupt service routine for ATmega328/328P.
     * 
     * This ISR is triggered when an interrupt occurs for the TWI interface, 
     * invoking the `isr` method of the TWI0 object.
     */
    ISR(TWI_vect)
    {
        TWI0.isr();  //*< Calls the isr method of TWI0 to handle TWI interrupts.
    }
#elif defined(__AVR_ATmega328PB__)
    /**
     * @brief Create an instance of the TWI interface for ATmega328PB.
     * 
     * This instantiates a __TWI__ object using the corresponding register pointers for 
     * ATmega328PB.
     */
    __TWI__ TWI0 = __TWI__(&TWBR0, &TWSR0, &TWAR0, &TWDR0, &TWCR0, &TWAMR0);

    /**
     * @brief TWI interrupt service routine for ATmega328PB.
     * 
     * This ISR is triggered when an interrupt occurs for the TWI interface, 
     * invoking the `isr` method of the TWI0 object.
     */
    ISR(TWI0_vect)
    {
        TWI0.isr();  //*< Calls the isr method of TWI0 to handle TWI interrupts.
    }
#else
    /**
     * @brief Error message if the TWI interrupt routine cannot be created.
     * 
     * This macro generates a compile-time error if none of the supported microcontrollers 
     * (ATmega328, ATmega328P, or ATmega328PB) are defined.
     */
    #error "Can't create a TWI bus 0 interrupt routine"  //*< Compile-time error if no valid microcontroller is selected.
#endif
