# include <avr/io.h>
# include <avr/interrupt.h>
# include "DXL.h"
# include "USART.h"
# include "CPU.h"

# ifndef GLOBAL_H
# define GLOBAL_H

# define MAX_BFR_LEN 253  // as maximum length is 0xFF=N+2
# define BROADCAST_ID 254
# define NULL 0

extern uint8_t TXpacket [MAX_BFR_LEN + 4];
extern uint8_t TXlength;
extern uint8_t TXID;
extern uint8_t TXinstruction;
extern uint8_t TXparameters[MAX_BFR_LEN];
extern volatile uint8_t RXheadersize;
extern volatile uint8_t RXparameters[MAX_BFR_LEN];
extern volatile uint8_t RXID;
extern volatile uint8_t RXlength;
extern volatile uint8_t RXdata;
extern volatile uint8_t RXerror;
extern volatile uint8_t RXchecksum;
extern volatile uint8_t RXparamCounter;
extern volatile uint8_t *RXparametersptr;
extern ReceiverState::State RXstate;

# endif