# include <avr/io.h>
# include <avr/interrupt.h>

# define MAX_BFR_LEN 259

uint8_t TXpacket [MAX_BFR_LEN] = {0xFF, 0xFF}; // as maximum length is 0xFF + checksum etc.
uint8_t TXlength; // number of parameters + 2
uint8_t TXID; // from 0 to 253, 254 is BROADCAST_ID
uint8_t TXinstruction;
volatile uint8_t RXbuffer[MAX_BFR_LEN];
uint8_t RXlength; // volatile?
volatile uint8_t *RXbufferptr = RXbuffer;

ISR(USART1_RX_vect) { // USART receive
	*RXbufferptr = UDR1; // reding UDR clears interrupt flag
	RXbufferptr++;
	TCNT1 = 0; // reset timer 1
}

ISR(TIMER1_COMPA_vect) {
	// TODO: check buffer
	// TODO: if ok: stop timer, leave interrupt
	// else:
	do {
		*RXbufferptr = 0;
	} while (RXbufferptr > RXbuffer);
	TCNT1 = 0; // reset timer 1
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

void USART_Init( unsigned int baud ) {
	/* Set baud rate */
	UBRR1 = (unsigned char)baud;
	/* Enable receiver and transmitter */
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1 << USBS1) | (3 << UCSZ10);
}

void USART_Transmit( unsigned char data ) {
	/* use for 5 to 8 data bits */
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1 << UDRE1)));
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

unsigned char USART_Receive( void ) {
	/* use for 5 to 8 data bits */
	/* Wait for data to be received */
	while ( !(UCSR1A & (1 << RXC1)) );
	/* Get and return received data from buffer */
	return UDR1;
}

void USART_Flush( void ) {
	/* flushes receive buffer */
	unsigned char dummy;
	while ( UCSR1A & (1 << RXC1) ) dummy = UDR1;
}

void USART_Parity (unsigned int state) {
	/* enables/disables insertion of parity bit between the last data bit and
	the first stop bit of the frame that is sent */
	if ((state != 1) || (state != 0)) return;

	UCSR1C = (state << UPM11);
}

int RX_ReceiveStatus() {
	if ( !(UCSR1A & (1 << RXC1)) ) return 0; //input buffer empty
	else return 1; //input buffer full
}

void TXRX_DataControl (uint8_t direction) {
	// sets the state of pin number 2
	// pin number 2 is PD1 in CPU
	switch (direction) {
	case 1: // output high - send
		PORTD = (1 << PD1);
		DDRD = (1 << DDD1);
		break;
	case 0: // output low - receive
		PORTD = (0 << PD1);
		DDRD = (1 << DDD1);
		break;
	}
}

uint8_t CalcHighbyte(int word) {
	return word >> 8;
}

uint8_t CalcLowbyte(int word) {
	return word && 0xFF;
}

class AXParameter {
public:
	enum Type : uint8_t {

		// ............

		ID = 0x03,
		BaudRate = 0x04,

		GoalPosition_LoByte = 0x1e,
		GoalPosition_HiByte = 0x1f,

		// .............
	};
};

class AXInstruction {
public:
	enum Type : uint8_t {
		Ping = 0x01,
		ReadData = 0x02,
		WriteData = 0x03,
		RegWrite = 0x04,
		Action = 0x05,
		Reset = 0x06,
		SyncWrite = 0x83
	};
};

void TX_SetLength(uint8_t length) {
	TXlength = length;
}

void TX_SetId(uint8_t ID) {
	TXID = ID;
}

void TX_SetInstruction(AXInstruction::Type instruction) {
	TXinstruction = instruction;
}

void TX_SetParameter(int index, AXParameter::Type parameter) {
	if (index >= 253) return;
	TXpacket[index] = parameter;
}

void DXL_Initialize() {
	USART_Init(0); // 1 000 000 baud
	for (int i = 0; i < MAX_BFR_LEN; i++) {
		RXbuffer[i] = 0;
		TXpacket[i] = 0;
	}
	CPU_SetUpTimer();
	CPU_EnableRXInterrupt ();
	sei(); // enable interrupts

}

int main () {
	for(;;); // keep program running
}

// current timer implemantation could be replaced with CTC timer (p.133)
// CTC timer could interrupt and incremant volatile uint8_t -> move from 16bit to 8 bit timer
