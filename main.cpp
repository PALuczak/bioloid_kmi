# include "global.h"

int main () {
	DDRC |= (1 << DDC7);
	//CPU_EnableRXInterrupt ();
	CPU_SetUpTimer();
	sei();
	for (;;) { //; // keep program running
		// if (TIFR1 & (1 << OCF1A)) {
		// 	PORTC ^= (1 << PORTC7);
		// 	TIFR1 = (1 << OCF1A); // clear CTC flag
		// }
	}
}
