#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char sreg;	// uses for temporarily saving register values

int main(void){
	// Setup clock and OC1A (pin 15)
	DDRB |= (1 << DDB1);	// Set PB1 (aka OC1A) as output
	PORTB &= ~(1 << PORTB1);	// Set PB1 low
	DDRB |= (1 << DDB2);	// Set PB2 (aka OC1B) as output
	PORTB &= ~(1 << PORTB2);	// Set PB2 low

	TCCR1A = 0xa0;	// Set OC1A & OC1B on match when upcounting
	TCCR1B = 0x11;	// Set internal clock source, set WGM13:2 for phase & freq correct PWM mode (WGM11:0 set in TCCR1A)
	sreg = SREG;
	cli();
	OCR1A = 0x00; OCR1B = 0x00;	// 16-bit register access, must be wrapped in disabling interrupts
	ICR1 = 0xff;	// Also 16-bit register. Set the values of TOP to 0xff;
	SREG = sreg;

	// Set OC1A to 20% duty cycle
	sreg = SREG;
	cli();
	OCR1A = 0x13;
	OCR1B = 0x33;
	SREG = sreg;

	return 0;
}
