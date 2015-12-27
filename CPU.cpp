# include "global.h"

ISR(USART1_RX_vect) { // USART receive
	TCNT1 = 0; // reset timer 1
	uint8_t status = UCSR1A; // check for errors (FEn, DORn, UPEn), reading this clears those flags
	RXdata = UDR1; // reading UDR clears interrupt flag

	if (status & (_BV(FE1) | _BV(DOR1) | _BV(UPE1))) {
		return;
	}

	switch (RXstate) {
	case ReceiverState::ReceiveHeader:
		RXchecksum = 0;
		RXparamCounter = 0;
		RXheadersize = 0;
		RXparametersptr = NULL;
		if (RXdata != 0xFF) {
			RXheadersize = 0;
			return;
		}
		else {
			RXheadersize++;
		}
		if (RXheadersize == 2) RXstate = ReceiverState::ReceiveID;
		break;
	case ReceiverState::ReceiveID:
		if (RXdata != 0xFF) { // ID of 255 impossible
			RXID = RXdata;
			RXchecksum += RXdata;
			RXstate = ReceiverState::ReceiveLength;
			return;
		}
		else {
			RXstate = ReceiverState::ReceiveHeader; // reset the receiver automata
		}
		break;
	case ReceiverState::ReceiveLength:
		RXlength = RXdata;
		RXchecksum += RXdata;
		RXstate = ReceiverState::ReceiveError;
		if (RXlength < 2) RXstate = ReceiverState::ReceiveHeader;
		break;
	case ReceiverState::ReceiveError:
		RXerror  = RXdata;
		RXchecksum += RXdata;
		RXstate = ReceiverState::ReceiveParameter;
		if (RXlength == 2) RXstate = ReceiverState::ReceiveChecksum;
		break;
	case ReceiverState::ReceiveParameter:
		if (RXparametersptr == NULL) RXparametersptr = RXparameters;
		*RXparametersptr = RXdata;
		RXparametersptr += sizeof(uint8_t);
		RXparamCounter++;
		RXchecksum += RXdata;
		if (RXparamCounter == RXlength - 2) RXstate = ReceiverState::ReceiveChecksum;
		break;
	case ReceiverState::ReceiveChecksum:
		RXchecksum = ~RXchecksum;
		if (RXdata != RXchecksum) RXstate = ReceiverState::ReceiveHeader;
		else RXstate = ReceiverState::Finished;
		break;
	default:
		break;
	}
}

ISR(TIMER1_COMPA_vect) {
	PORTC ^= (1 << PORTC7);
	RXstate = ReceiverState::ReceiveHeader;
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
