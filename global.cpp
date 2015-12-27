# include "global.h"

uint8_t TXpacket [MAX_BFR_LEN + 4];
uint8_t TXlength; // number of parameters + 2
uint8_t TXID; // from 0 to 253, 254 is BROADCAST_ID
uint8_t TXinstruction;
uint8_t TXparameters[MAX_BFR_LEN];
volatile uint8_t RXheadersize = 0;
volatile uint8_t RXparameters[MAX_BFR_LEN];
volatile uint8_t RXID = 0x00;
volatile uint8_t RXlength = 0x00;
volatile uint8_t RXdata = 0;
volatile uint8_t RXerror = 0x00;
volatile uint8_t RXchecksum = 0x00;
volatile uint8_t RXparamCounter = 0;
volatile uint8_t *RXparametersptr = RXparameters;
ReceiverState::State RXstate = ReceiverState::ReceiveHeader;