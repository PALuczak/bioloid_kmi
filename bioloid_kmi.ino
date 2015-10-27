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

int TXRX_ReceivePacket (uint8_t storage[]) {
	uint8_t length = 0;
	uint8_t expectedlen = 0;
	uint8_t temp;
	bool fullhead = false;
	uint8_t checksum;

	while (!(temp = (uint8_t) USART_Receive())) { // wait for input | is this a correct way of doing it?
		storage = (uint8_t*)realloc(storage, (length + 1) * sizeof(uint8_t));
		storage[length] = temp;
		length++;
		if (fullhead == false) { //header
			if (temp == 0xFF) {
				if (length == 2) fullhead = true;
				continue;
			}
		}
		if (length == 3) continue; // ID
		if (length == 4) { //length
			expectedlen = temp + length;
			continue;
		}
		if (length < expectedlen) continue; // instruction + parameters
		if (length == expectedlen) { //checksum
			for (length = 1; length < expectedlen; length++) {
				checksum += storage[length - 1];
			}
			checksum = ~checksum;
			if (checksum == temp) {
				return 1; //expectedlen;
			}
			else return 0;
		}
	}
}

// above functions will be moved to a private method in DXL class once communication is achieved
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

void FindBaud () { //find what baud is set in the servo
	int baud_param[] = {103, 51, 16, 8, 0xFF, 3, 0xFF, 1, 0};
	long baud_val[] = {9600, 19200, 57600, 115200, 200000, 250000, 400000, 500000, 1000000};
	int index = 0;
	uint8_t packet[6] = {0xFF, 0xFF, 0xFE, 0x02, 0x01, 0xFE};
	int packindex = 0;
	uint8_t* answer = NULL;

	// send ping packet
	for (index = 0; index < 9; index++) {
		USART_Init (baud_param[index]);
		for (packindex = 0; packindex < 6; packindex++) {
			USART_Transmit(packet[packindex]);
		}
		delay(1);
		if (TXRX_ReceivePacket(answer) == 1) { //found baud
			USART_Init(103);
			USART_Transmit(0x00);
			USART_Transmit(0xFF);
			USART_Transmit(0x00);
			USART_Transmit(baud_param[index]);
			USART_Transmit(0x00);
			USART_Transmit(0xFF);
			USART_Transmit(0x00);
			//return; // find only one baudrate
		}
		delay(1000);
	}
	USART_Init(103); // not found baud
	USART_Transmit(0x00);
	USART_Transmit(0xFF);
	USART_Transmit(0x00);
	USART_Transmit(0xAA);
	USART_Transmit(0x00);
	USART_Transmit(0xFF);
	USART_Transmit(0x00);
}
//----------------------------------------

void setup() {
	//USART_Init(103); // 9600
	//USART_Init(51); // 19.2K
	//USART_Init(16); // 57.6 K
	//USART_Init(8); // 115.2k
	//ATMEL manual: p208, p191

	// find the baud of the servo (set, send, wait for response)

	/* nie musisz za każdym razem ustawiać prędkości transmisji.
	  zobacz w dokumentacji, że parametr 4 jest w obszarze pamięci EEPROM,
	  a zatem wartość predkości transmisji jest pamiętana po ponownym włączeniu serwonapędu.
	  Co wiecej, takie częste zapisaywanie parametrów do EEPROMa spowoduje jego uszkodzenie (wypalenie).
	 */

	// set baud in servo
	/*USART_Transmit(0xFF);
	USART_Transmit(0xFF); // header
	USART_Transmit(0x01); // id
	USART_Transmit(0x04); // length
	USART_Transmit(0x03); // instruction
	USART_Transmit(0x04); // baud
	USART_Transmit(0xCF); // value (207 in decimal) //9600
	//USART_Transmit(0x10); // 16 in decimal // 115.2k
	USART_Transmit(0x24); // checksum
	*/
	FindBaud();
}

uint8_t tx_buffer[32] = {0xFF, 0xFF};
uint8_t* tx_buffe_write_ptr = NULL;

class AXInstruction
{
public:
	enum Type : uint8_t
	{
		Ping = 0x01,
		ReadData = 0x02,
		WriteData = 0x03,
		RegWrite = 0x04,
		Action = 0x05,
		Reset = 0x06,
		SyncWrite = 0x83
	};
};

class AXParameter
{
public:
	enum Type : uint8_t
	{

		// ............

		ID = 0x03,
		BaudRate = 0x04,

		GoalPosition_LoByte = 0x1e,
		GoalPosition_HiByte = 0x1f,

		// .............
	};
};


void ax_begin_message(uint8_t dev_id, AXInstruction::Type instruction)
{

	tx_buffer[2] = dev_id;
	//tx_buffer[3] = 0;
	tx_buffer[4] = (uint8_t)instruction;
	tx_buffe_write_ptr = tx_buffer + 5;
	//return tx_buffe_write_ptr;

}

inline static void ax_message_set_parameter(AXParameter::Type param, uint8_t value)
{
	tx_buffe_write_ptr[0] = (uint8_t)param;
	tx_buffe_write_ptr[1] = value;
	tx_buffe_write_ptr += 2;
}

uint8_t ax_end_message(void)
{
	// okresl dlugosc pakietu
	tx_buffer[3] = 2 + tx_buffe_write_ptr - (tx_buffer + 5);

	// policz sumę kontrolną i dopisz na koncu wiadomosci
	uint8_t crc = 0;
	uint8_t* ptr = tx_buffer + 2;
	uint8_t len = tx_buffer[3] + 2;
	while (len > 0)
		crc += *ptr++;
	*tx_buffe_write_ptr = (uint8_t)~crc;


	// zwroc CAŁKOWITĄ długość bufora do wysłania
	return (tx_buffe_write_ptr - tx_buffer) + 1;
}

void ax_send_message(void)
{
	uint8_t len = tx_buffer[3] + 4;
	uint8_t* ptr = tx_buffer;

	while (len > 0)
		USART_Transmit(*ptr++);
}


void loop() {
	//USART_Transmit('2');
	//TXRX_SendString("Test String, please ignore\n\r");
	//TXRX_SendHex(15);
	// write

	/* ok, ale zanim ustawisz pozycję docelową to pytanie:
	   w jakim trybiej jest ta pozycja?? czy to ma być tryb położenia czy prędkości?
	   Skąd wiesz, w jakim trybie jest teraz serwo.
	 */


	/*USART_Transmit(0xFF);
	USART_Transmit(0xFF); // header
	USART_Transmit(0x01); // id
	USART_Transmit(0x06); // length
	USART_Transmit(0x03); // instruction
	USART_Transmit(0x1E); // goal position low
	USART_Transmit(0x93); // value
	USART_Transmit(0x1F); // goal position high
	USART_Transmit(0x00); // value
	USART_Transmit(0x25); // checksum


	// jeszcze raz to samo, tylko w ludziej postaci :)
	/*uint16_t position = 0x55;

	ax_begin_message(0x01, AXInstruction::WriteData);
	ax_message_set_parameter(AXParameter::GoalPosition_LoByte, position & 0xFF);
	ax_message_set_parameter(AXParameter::GoalPosition_HiByte, (position >> 8) & 0xFF);
	ax_end_message();

	ax_send_message();*/


	/*
	 brakuje mi kodu

	 tak - AX12 wymaga łącza half duplex. Brakuje kodu, który sterowałby przełączenikiem TTL od kierunku transji
	 to ma działać tak:
	 Pin sterujący ustawiasz wtryb Arduino --> serwo i odpalasz ax_send_message()
	 Zaraz po zakoczeniu wewnętrznej petli pin sterujący nalezy przełączyć w tryb arduino <-- serwo i rozpoczac oczekiwanie na dane.
	 Przy czym oczekiwanie musi być podparte czasem (timeout) lub przerwaniem i czasem (najlepsze, ale bardziej złożone rozwiązanie)

	*/
	//delay(1000);
}

