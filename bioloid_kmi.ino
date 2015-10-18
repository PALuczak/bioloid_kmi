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

// written by me
void USART_Parity (unsigned int state) {
	/* enables/disables insertion of parity bit between the last data bit and
	the first stop bit of the frame that is sent */
	if ((state != 1) || (state != 0)) return;

	UCSR1C = (state << UPM11);
}

void TXRX_SendString (char* string) {
	int letter = 0;

	while (string[letter] != '\0') {
		USART_Transmit(string[letter]);
		letter++;
	}
}

void TXRX_ReceiveString (char* storage) {
	int length = 0; //initial length
	char temp = '\0';

	do {
		temp = USART_Receive();
		storage = (char*)realloc(storage, (length + 1) * sizeof(char));
		storage[length] = temp;
		length++;
	} while (temp != '\0');
}
// above functions will be moved to a private method in DXL class
class DXL {
private:
	uint8_t* TXpacket;
	uint8_t TXlength;
	uint8_t TXID;
	uint8_t TXinstruction;
	uint8_t* TXparameter;
	int TXparameterlength;

	uint8_t* RXpacket;

	uint8_t CalcChecksum(int bytes, int *bytevals[]) {
		int checksum = 0;
		for (int counter = 0; counter < bytes; counter++) {
			checksum += (*bytevals)[counter];
		}
		if (checksum < 0xFF) {
			checksum = CalcLowbyte(checksum);
		}
		return (uint8_t)~checksum;
	}
public:
	void TXSetLength(uint8_t length) {
		TXlength = length;
	}
	void TXSetId(uint8_t ID) {
		TXID = ID;
	}
	void TXSetInstruction(uint8_t instruction) {
		TXinstruction = instruction;
	}
	void TXSetParameter(uint8_t index, uint8_t value) {
		if (index >= TXparameterlength) {
			TXparameter = (uint8_t*) realloc(TXparameter, (index + 1) * sizeof(uint8_t));
			TXparameterlength = index + 1;
		}
		*(TXparameter + index) = value;
	}

	int RXGetLength();
	int RXGetError();
	int RXGetParameter();

	void TXSend();
	void RXReceive();

	// uint8_t CalcHighbyte(int word) {
	// 	return word >> 8;
	// }
	// uint8_t CalcLowbyte(int word) {
	// 	return word && 0xFF;
	// }

	DXL() {
		TXpacket = NULL;
		RXpacket = NULL;
		TXparameter = NULL;
		TXinstruction = 0;
		TXID = 0;
		TXlength = 0;
		TXparameterlength = 0;
	}

	~DXL() {
		free(TXpacket);
		free(RXpacket);
		free(TXparameter);
		TXpacket = NULL;
		RXpacket = NULL;
		TXparameter = NULL;
	}
};

uint8_t CalcHighbyte(int word) {
	return word >> 8;
}
uint8_t CalcLowbyte(int word) {
	return word && 0xFF;
}

//----------------------------------------

void setup() {
	USART_Init(103);
	//ATMEL manual: p208, p191

	// set baud in servo
	USART_Transmit(0xFF);
	USART_Transmit(0xFF); // header
	USART_Transmit(0x03); // id
	USART_Transmit(0x04); // length
	USART_Transmit(0x03); // instruction
	USART_Transmit(0x04); // baud
	USART_Transmit(0xCF); // value (207 in decimal)
	USART_Transmit(0x22); // checksum
}

void loop() {
	//USART_Transmit('2');
	//TXRX_SendString("Test String, please ignore\n\r");
	//TXRX_SendHex(15);
	// write
	USART_Transmit(0xFF);
	USART_Transmit(0xFF); // header
	USART_Transmit(0x03); // id
	USART_Transmit(0x06); // length
	USART_Transmit(0x03); // instruction
	USART_Transmit(0x30); // goal position low
	USART_Transmit(0x33); // value
	USART_Transmit(0x31); // goal position high
	USART_Transmit(0x00); // value
	USART_Transmit(0x5F); // checksum

	// the above attempt to comunicate with servo failed
	// possibly due to bad baudrate or another unknown problem
	delay(1000);
}

