# include <avr/io.h>
# include <avr/interrupt.h>

void DXL_Initialize();
void DXL_Receive();
void DXL_SendBit(unsigned char data);
void DXL_SendPacket(unsigned char packet[], int packetLength);