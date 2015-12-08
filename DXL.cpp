# include <avr/io.h>
# include <avr/interrupt.h>
//# include "DXL.h"
# include "global.h"

void DXL_Initialize() {
	USART_Init(0); // 1 000 000 baud
	for (int i = 0; i < MAX_BFR_LEN; i++) {
		RXbuffer[i] = 0;
		TXpacket[i] = 0;
	}
	CPU_SetUpTimer();
	CPU_EnableRXInterrupt();
	sei(); // enable interrupts
}