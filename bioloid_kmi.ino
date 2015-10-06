// #include <DynamixelSerial.h>
// # include <Arduino.h>
# include <avr/io.h>

// from ATmega datasheet
void USART_Init( unsigned int baud ) {
	/* Set baud rate */
	//UBRRH = (unsigned char)(baud>>8);
	//UBRRL = (unsigned char)baud;
	UBRR1 = (unsigned char)baud;
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}
void USART_Transmit( unsigned char data ) {
	/* use for 5 to 8 data bits */
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)));
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

unsigned char USART_Receive( void ) {
	/* use for 5 to 8 data bits */
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) );
	/* Get and return received data from buffer */
	return UDR1;
}

void USART_Flush( void ) {
	/* flushes receive buffer */
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}

// written by me
void USART_Parity (unsigned int state) {
	/* enables/disables insertion of parity bit between the last data bit and 
	the first stop bit of the frame that is sent */
	if((state != 1) || (state != 0)) return;

	UCSR1C = (state << UPM11);
}

//----------------------------------------

void setup() {
//   Dynamixel.begin(1000000,2); //2=data control
	USART_Init(103);
}

void loop() {
	int position = 500;

  Dynamixel.move(3,position);
	USART_Transmit('3');
	USART_Transmit('8');
	delay(100);
}

