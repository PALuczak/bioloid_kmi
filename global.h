# include <avr/io.h>
# include <avr/interrupt.h>
# include "DXL.h"
# include "USART.h"
# include "CPU.h"

# ifndef GLOBAL_H
# define GLOBAL_H

# define MAX_BFR_LEN 259  // as maximum length is 0xFF + checksum etc.
# define BROADCAST_ID 254

extern uint8_t TXpacket [MAX_BFR_LEN];
extern uint8_t TXlength;
extern uint8_t TXID;
extern uint8_t TXinstruction;
extern volatile uint8_t RXbuffer[MAX_BFR_LEN];
extern volatile uint8_t RXlength;
extern volatile uint8_t RXdata;
extern volatile uint8_t *RXbufferptr;
extern volatile uint8_t *RXbufferendptr;
extern ReceiverState::State RXstate;

# endif