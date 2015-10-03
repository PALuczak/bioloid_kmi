// # include <DynamixelSerial.h>
// # include <Arduino.h>
# include <avr/io.h>

// from ATmega datasheet
void USART_Transmit( unsigned char data ) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)));
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void USART_Init( unsigned int baud ) {
	/* Set baud rate */
	//UBRRH = (unsigned char)(baud>>8);
	//UBRRL = (unsigned char)baud;
	UBRR1 = (unsigned char)baud;
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* 1Set frame format: 8data, 2stop bit */
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}

//----------------------------------------

void setup() {
  // Dynamixel.begin(1000000,2); //2=data control

	USART_Init(9600);
}

void loop() {
	int position = 500;

  //Dynamixel.move(3,position);

	USART_Transmit('1');
	USART_Transmit('\n');
	delay(100);
}
