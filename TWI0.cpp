/* Dependencies */
#include "TWI.h"

#if defined(HAS_TWI)

#if defined(__AVR_ATmega328P__)
__TWI__ TWI = __TWI__(&TWBR, &TWSR, &TWAR, &TWDR, &TWCR, &TWAMR);
ISR(TWI_vect)
#elif defined(__AVR_ATmega328PB__)
__TWI__ TWI = __TWI__(&TWBR0, &TWSR0, &TWAR0, &TWDR0, &TWCR0, &TWAMR0);
ISR(TWI0_vect)
#endif
{
	TWI.isr();
}

#endif
