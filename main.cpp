# include "global.h"

int main () {
	DDRC |= (1 << DDC7);
	CPU_EnableRXInterrupt ();
	CPU_SetUpTimer();
	sei();
	for (;;); // keep program running
}
