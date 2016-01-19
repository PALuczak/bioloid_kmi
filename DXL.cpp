# include "global.h"

void DXL_Initialize() {
	USART_Init(0); // 1 000 000 baud
	CPU_SetUpTimer();
	CPU_EnableRXInterrupt();
	sei(); // enable interrupts
}

void DXL_Receive() {
	RXstate = ReceiverState::ReceiveHeader;
	TXRX_DataControl(0);
}

void DXL_SendBit(unsigned char data) {
	TXRX_DataControl(1);
	USART_Transmit(data);
	TXRX_DataControl(0);
}

void DXL_SendPacket(unsigned char packet[], int packetLength) {
	TXRX_DataControl(1);
	for (int i = 0; i < packetLength; i++) {
		USART_Transmit(packet[i]);
	}
	_delay_us(100);
	TXRX_DataControl(0);
}