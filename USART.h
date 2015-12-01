# include <avr/io.h>
# include <avr/interrupt.h>

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

void USART_Init( unsigned int baud );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void USART_Flush( void );
void USART_Parity (unsigned int state);
int RX_ReceiveStatus();
void TXRX_DataControl (uint8_t direction);
void TX_SetLength(uint8_t length);
void TX_SetId(uint8_t ID);
void TX_SetInstruction(AXInstruction::Type instruction);
void TX_SetParameter(int index, AXParameter::Type parameter);

uint8_t CalcHighbyte(int word);
uint8_t CalcLowbyte(int word);