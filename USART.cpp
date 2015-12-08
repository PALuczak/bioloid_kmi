# include <avr/io.h>
# include <avr/interrupt.h>
# include "global.h"

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
	(void)dummy; // silence compiler warning
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