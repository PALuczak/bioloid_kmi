# include <avr/io.h>
# include <avr/interrupt.h>
# include "DXL.h"
# include "USART.h"
# include "CPU.h"

# ifndef GLOBAL_H
# define GLOBAL_H

# define MAX_BFR_LEN 259

extern uint8_t TXpacket [MAX_BFR_LEN];
extern uint8_t TXlength;
extern uint8_t TXID;
extern uint8_t TXinstruction;
extern volatile uint8_t RXbuffer[MAX_BFR_LEN];
extern volatile uint8_t RXlength;
extern volatile uint8_t *RXbufferptr;
extern volatile uint8_t *RXbufferendptr;

# endif