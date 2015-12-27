# include "global.h"

void DXL_Initialize() {
	USART_Init(0); // 1 000 000 baud
	CPU_SetUpTimer();
	CPU_EnableRXInterrupt();
	sei(); // enable interrupts
}