/* Dependencies */
#include "TWI.h"

/**
 * @brief Initializes the TWI1 interface for the ATmega328PB microcontroller.
 * 
 * This code section initializes the TWI (Two-Wire Interface) for the ATmega328PB microcontroller, 
 * setting up the corresponding registers and assigning an interrupt service routine (ISR) to 
 * handle TWI1 interrupts.
 */
#if defined(__AVR_ATmega328PB__)
    /**
     * @brief Create an instance of the TWI1 interface for ATmega328PB.
     * 
     * This instantiates a __TWI__ object using the corresponding register pointers for 
     * TWI1 on the ATmega328PB microcontroller.
     */
    __TWI__ TWI1 = __TWI__(&TWBR1, &TWSR1, &TWAR1, &TWDR1, &TWCR1, &TWAMR1);

    /**
     * @brief TWI1 interrupt service routine for ATmega328PB.
     * 
     * This ISR is triggered when an interrupt occurs for the TWI1 interface, 
     * invoking the `isr` method of the TWI1 object.
     */
    ISR(TWI1_vect)
    {
        TWI1.isr();  //*< Calls the isr method of TWI1 to handle TWI1 interrupts.
    }
#else
    /**
     * @brief Error message if the TWI1 interrupt routine cannot be created.
     * 
     * This macro generates a compile-time error if the ATmega328PB microcontroller is not defined, 
     * preventing the creation of the TWI1 interrupt routine.
     */
    #error "Can't create a TWI bus 1 interrupt routine"  //*< Compile-time error if no valid microcontroller (ATmega328PB) is selected.
#endif

