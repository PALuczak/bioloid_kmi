# include <avr/io.h>
# include <avr/interrupt.h>
# include "global.h"

uint8_t TXpacket [MAX_BFR_LEN];
uint8_t TXlength; // number of parameters + 2
uint8_t TXID; // from 0 to 253, 254 is BROADCAST_ID
uint8_t TXinstruction;
volatile uint8_t RXbuffer[MAX_BFR_LEN];
volatile uint8_t RXlength = 0;
volatile uint8_t RXdata = 0;
volatile uint8_t *RXbufferptr = RXbuffer;
volatile uint8_t *RXbufferendptr = &RXbuffer[MAX_BFR_LEN];
ReceiverState::State RXstate = ReceiverState::ReceiveHeader;