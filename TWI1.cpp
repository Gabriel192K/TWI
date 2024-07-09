/* Dependencies */
#include "TWI.h"

#if defined(HAS_TWI1)

#if defined(__AVR_ATmega328PB__)
__TWI__ TWI1 = __TWI__(&TWBR1, &TWSR1, &TWAR1, &TWDR1, &TWCR1, &TWAMR1);
ISR(TWI1_vect)
{
	TWI.isr();
}
#endif
#endif
