# include "global.h"
# include <stdlib.h>
#define RAND_MAX 0x3FF

void Diode () {
	uint8_t packet1[] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x19, 0x01, 0xDD};
	uint8_t packet2[] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x19, 0x00, 0xDE};
	uint8_t packet3[] = {0xFF, 0xFF, 0x02, 0x04, 0x03, 0x19, 0x01, 0xDC};
	uint8_t packet4[] = {0xFF, 0xFF, 0x02, 0x04, 0x03, 0x19, 0x00, 0xDD};

	while(1){
		DXL_SendPacket(packet1,sizeof(packet1)/sizeof(unsigned char));
		DXL_SendPacket(packet3,sizeof(packet3)/sizeof(unsigned char));
		_delay_ms(100);
		DXL_SendPacket(packet2,sizeof(packet2)/sizeof(unsigned char));
		DXL_SendPacket(packet4,sizeof(packet4)/sizeof(unsigned char));
		_delay_ms(100);
	}
}

void MoveRandom() {
	uint8_t SyncPacket[22];
	int target = 0;

	while (1) {
		target = rand();
		SyncPacket[0] = 0xFF;
		SyncPacket[1] = 0xFF;
		SyncPacket[2] = 0xFE;
		SyncPacket[3] = 0x14;
		SyncPacket[4] = 0x83;
		SyncPacket[5] = 0x1E;
		SyncPacket[6] = 0x06;
		SyncPacket[7] = 0x01;
		SyncPacket[8] = CalcLowbyte(target);
		SyncPacket[9] = CalcHighbyte(target);
		SyncPacket[10] = 0x00;
		SyncPacket[11] = 0x02;
		SyncPacket[12] = 0x00;
		SyncPacket[13] = 0x02;
		SyncPacket[14] = 0x02;
		SyncPacket[15] = CalcLowbyte(~target);
		SyncPacket[16] = CalcHighbyte(~target);
		SyncPacket[17] = 0x00;
		SyncPacket[18] = 0x02;
		SyncPacket[19] = 0x00;
		SyncPacket[20] = 0x02;
		SyncPacket[21] = 0;
		for (int i = 2; i < 21; i++)
			SyncPacket[21] += SyncPacket[i];
		SyncPacket[21] = ~SyncPacket[21];
		
		DXL_SendPacket(SyncPacket,sizeof(SyncPacket)/sizeof(unsigned char));
		_delay_ms(1000);
	}
}

void Servo () {
	uint8_t packet1[] = {0xFF, 0xFF, 0x01, 0x09, 0x03, 0x1E, 0x09, 0x00, 0x00, 0x02, 0x00, 0x02, 0xC7};
	uint8_t packet2[] = {0xFF, 0xFF, 0x01, 0x09, 0x03, 0x1E, 0xF0, 0x03, 0x00, 0x02, 0x00, 0x02, 0xDD};
	uint8_t packet3[] = {0xFF, 0xFF, 0x02, 0x09, 0x03, 0x1E, 0x09, 0x00, 0x00, 0x02, 0x00, 0x02, 0xC6};
	uint8_t packet4[] = {0xFF, 0xFF, 0x02, 0x09, 0x03, 0x1E, 0xF0, 0x03, 0x00, 0x02, 0x00, 0x02, 0xDC};
	while(1){
		DXL_SendPacket(packet1,sizeof(packet1)/sizeof(unsigned char));
		DXL_SendPacket(packet3,sizeof(packet3)/sizeof(unsigned char));
		_delay_ms(1000);
		DXL_SendPacket(packet2,sizeof(packet2)/sizeof(unsigned char));
		DXL_SendPacket(packet4,sizeof(packet4)/sizeof(unsigned char));
		_delay_ms(1000);
	}
}

int main () {
	DDRC |= (1 << DDC7);
	//DDRD |= (1 << DDD7);

	USART_Init(0);
	//Diode();
	MoveRandom();
	//Servo();
	for (;;); // keep program running
}