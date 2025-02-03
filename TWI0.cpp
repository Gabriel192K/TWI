/* Dependencies */
#include "TWI.h"

#if defined(__AVR_ATmega328__)  || \
	defined(__AVR_ATmega328P__)
__TWI__ TWI0 = __TWI__(&TWBR, &TWSR, &TWAR, &TWDR, &TWCR, &TWAMR);
ISR(TWI_vect)
{
	TWI0.isr();
}
#elif defined(__AVR_ATmega328PB__)
__TWI__ TWI0 = __TWI__(&TWBR0, &TWSR0, &TWAR0, &TWDR0, &TWCR0, &TWAMR0);
ISR(TWI0_vect)
{
	TWI0.isr();
}
#else
#error "Can't create an TWI bus 0 interrupt routine"
#endif