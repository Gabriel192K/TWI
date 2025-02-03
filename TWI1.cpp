/* Dependencies */
#include "TWI.h"

#if defined(__AVR_ATmega328PB__)
__TWI__ TWI1 = __TWI__(&TWBR1, &TWSR1, &TWAR1, &TWDR1, &TWCR1, &TWAMR1);
ISR(TWI1_vect)
{
	TWI1.isr();
}
#else
#error "Can't create an TWI bus 1 interrupt routine"
#endif
