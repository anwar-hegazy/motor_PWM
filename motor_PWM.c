#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU	8000000UL	// 8Mhz cpu
#include <util/delay.h>

unsigned char sreg;	// uses for temporarily saving register values
volatile uint8_t i;	// Dummy counter variable for use with data buffer
volatile uint8_t dataRecBuff[4];	// Stores incoming data

ISR(TWI_vect){
	sreg = SREG; cli();	// Disable other interrupts
	if(TWSR == 0x60) {	// SLA+W received
		i = 0;		// Start at beginning of buffer again
	}
	if(TWSR == 0x80) {	// Data received and acknowledged
		if(i<4)	// Ignore extra data
			dataRecBuff[i] = TWDR;
		i++;
	}
	TWCR &= ~(0 << TWINT);	// Clear the interrupt flag (by writing 1 to it)
	SREG = sreg;	// Re-enable interrupts
}

void delayms(uint16_t millis){
	while(millis){
		_delay_ms(1);
		millis--;
	}
}

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

	// Empty the dataRecBuff
	for(i=0; i<4; i++){
		dataRecBuff[i] = 0x00;
	}

	/*
	// Set OC1A to 20% duty cycle
	sreg = SREG;
	cli();
	OCR1A = 0x13;
	OCR1B = 0x33;
	SREG = sreg;
	*/

	// Setup device as a slave receiver for TWI
	DDRC &= ~(1 << DDC4); // Configure the SDA pin as a pull-up
	PORTC |= 1 << PORTC4;
	DDRC &= ~(1 << DDC5); // Configure the SCL pin as a pull-up
	PORTC |= 1 << PORTC5;
	PRR &= ~(1 << PRTWI);	// Set the PRTWI bit to zero (ie. disable power-saving mode)
	TWAR = 0xa << 1;	// Set the address of the chip to 10, and TWGCE bit to zero (ignore general calls)
	TWCR = 0x45;	// Enable TWI with appropriate settings for slave receiver, enabling interrupts
	sei();	// Enable interrupts

	while(1){	// Idle loop to test code is still running
		// Set OC1A and OC1B to the duty cycles passed over TWI
		sreg = SREG;
		cli();
		OCR1A = dataRecBuff[0];
		OCR1B = dataRecBuff[1];
		SREG = sreg;
	}

	return 0;
}
