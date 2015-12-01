# include <avr/io.h>
# include <avr/interrupt.h>
//# include "CPU.h"
# include "global.h"

class ReceiverState {
public:
	enum State {
		ReceiveHeader,
		ReceiveID,
		ReceiveLength,
		ReceiveError,
		ReceiveParameter,
		ReceiveChecksum,
		Finished
	};
};

ReceiverState::State RXstate = ReceiverState::ReceiveHeader;

ISR(USART1_RX_vect) { // USART receive
	TCNT1 = 0; // reset timer 1
	uint8_t status = UCSR1A; // check for errors (FEn, DORn, UPEn), reading this clears those flags
	*RXbufferptr = UDR1; // reading UDR clears interrupt flag
	RXlength++;
	RXbufferptr++;
}

ISR(TIMER1_COMPA_vect) {
	TCNT1 = 0; // reset timer 1

	switch (RXstate) {
	case ReceiverState::ReceiveHeader:
		break;
	case ReceiverState::ReceiveID:
		break;
	default:
		break;
	}

	// TODO: check buffer
	// TODO: if ok: stop timer, leave interrupt
	// else:
	RXbufferptr = RXbuffer;
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
	TCCR1B |= ((1 << CS11) | (1 << CS12)); // set up prescaler = 64 (p.134 manual)
	OCR1A = 250 * 100; // 250 = 1 ms // set up output compare value for interrupt
	TIMSK1 |= (1 << OCIE1A); // enable interrupt on clock compare
}