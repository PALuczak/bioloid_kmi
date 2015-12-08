# include <avr/io.h>
# include <avr/interrupt.h>

class ReceiverState {
public:
	enum State {
		ReceiveHeader,
		ReceiveID,
		ReceiveLength,
		ReceiveError,
		ReceiveParameter,
		ReceiveChecksum,
		Finished
	};
};

void CPU_SetInterruptFlag();
void CPU_EnableRXInterrupt ();
void CPU_SetUpTimer();