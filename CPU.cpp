# include <avr/io.h>
# include <avr/interrupt.h>
//# include "CPU.h"
# include "global.h"

ISR(USART1_RX_vect) { // USART receive
	TCNT1 = 0; // reset timer 1
	uint8_t status = UCSR1A; // check for errors (FEn, DORn, UPEn), reading this clears those flags
	(void)status; // silence compiler warning -Wno-unused-variable flag is an alternative
	RXdata = UDR1; // reading UDR clears interrupt flag

	switch (RXstate) {
	case ReceiverState::ReceiveHeader:
		if (RXdata != 0xFF) {
			RXbufferptr = RXbuffer;
			return;
		}
		else {
			*RXbufferptr = RXdata;
			RXbufferptr += sizeof(uint8_t);
		}
		if (((RXbufferptr - RXbuffer) / sizeof(uint8_t)) == 2) RXstate = ReceiverState::ReceiveID;
		break;
	case ReceiverState::ReceiveID:
		if (RXdata != 0xFF) { // ID of 255 impossible
			RXbufferptr = RXbuffer;
			RXbufferptr += sizeof(uint8_t);
			RXstate = ReceiverState::ReceiveLength;
			return;
		}
		else {
			RXbufferptr = RXbuffer;
		}
		break;
	case ReceiverState::ReceiveLength:
		*RXbufferptr = RXdata;
		RXbufferptr += sizeof(uint8_t);
		RXstate = ReceiverState::ReceiveError;
		break;
	case ReceiverState::ReceiveError:

		break;
	case ReceiverState::ReceiveParameter:
		break;
	case ReceiverState::ReceiveChecksum:
		break;
	case ReceiverState::Finished:
		break;
	default:
		break;
	}
}

ISR(TIMER1_COMPA_vect) {
	// temporary code to test if the timer works:

	PORTC ^= (1 << PORTC7);
}

void CPU_SetInterruptFlag() { // unused as avr lib has sei() implemented
	unsigned char sreg = 0;
	sreg = SREG;
	sreg = sreg | 0b10000000; // set the I bit
	SREG = sreg;
}

void CPU_EnableRXInterrupt () {
	/*unsigned char reg = 0;
	reg = UCSR1B;
	reg = reg | 0b10000000; // set RXCIE1 bit
	UCSR1B = reg;*/
	UCSR1B |= (1 << RXCIE1);
}

void CPU_SetUpTimer() {
	OCR1A = 6249; // 100 ms set up output compare value for interrupt
	TIMSK1 |= (1 << OCIE1A); // enable interrupt on clock compare
	TCCR1B |= (1 << WGM12); // CTC mode
	TCCR1B |= ((0 << CS10) | (0 << CS11) | (1 << CS12)); // set up prescaler = 256 (p.134 manual)
}