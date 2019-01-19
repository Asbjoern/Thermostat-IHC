//#include "IHCOutput.h"
#include <Arduino.h>
#include <TimerOne.h>
#include "IHCsend.h"


void IHCinit() {
	memset(&outdata, 0, sizeof(outdata));
	bitpos = 0;
	starttime = 0;
  pinMode(OUT1, OUTPUT);
  digitalWrite(OUT1,LOW);
  pinMode(OUT2, OUTPUT);
  digitalWrite(OUT2,LOW);
  pinMode(OUT3, OUTPUT);
  digitalWrite(OUT3,LOW);
  Timer1.initialize(1000);//122000/3);
  Timer1.attachInterrupt(IHCTick);
}

word NibbleChecksum(int n) {

	return ((n >> 8) & 0x000f) + ((n >> 4) & 0x000f) + (n & 0x000f);
}

static void IHCfillBuffer() {

  for(int i=0;i<3;i++){
  	word checksum_1 = NibbleChecksum(val1[i]);
  	word checksum_2 = NibbleChecksum(val2[i]);
  	word checksum_3 = NibbleChecksum(val3[i]);
  	word checksum_5bit = ((checksum_1 + checksum_2 + checksum_3) & 0x000F);
  	memset(&outdata[i], 0, sizeof(outdata[i]));
  	bitpos = 0;
  	IHCAddBits(i,val1[i]);
  	IHCAddBits(i,val2[i]);
  	IHCAddBits(i,val3[i]);
  	IHCAddBits(i,checksum_5bit, 5);
  }
	starttime = millis();
  digitalWrite(OUT1,LOW);
  digitalWrite(OUT2,LOW);
  digitalWrite(OUT3,LOW);
}

void IHCsetData(Sensordata data){
  val1[0] = int(data.roomTemp*10.0 + 0.5);
  val2[0] = int(data.setpointTemp*10.0 + 0.5);
  val3[0] = int(data.floorTemp*10.0 + 0.5);
  val1[1] = val1[0];
  val2[1] = (data.lux>>5) & 0xfff;
  val3[1] = data.lux & 0x1f;
  val1[2] = val1[0];
  val2[2] = int(data.humidity*10.0 + 0.5);
  val3[2] = int((data.roomTemp-(100.-data.humidity)/5.)*10.0 + 0.5); //Simplified dew point calculation
}

int state=LOW;
static void IHCTick(void) {

	if (starttime == 0) {
		IHCfillBuffer();
		return;
	}
	unsigned long time = millis();
	long dt = time - starttime;
	int bitnr = dt / 114;
	if (bitnr >= 41) {
		// Wait 8 sec until next update
		if (dt > 5000) starttime = 0;
		return;
	}
	int t = dt % 114;
	int bp = 0;
	if (t >= 41) bp = 1;
	if (t >= 81) bp = 2;
	switch (bp) {
	case 0:
		digitalWrite(OUT1,HIGH);
		digitalWrite(OUT2,HIGH);
		digitalWrite(OUT3,HIGH);
		break;
	case 1: {
		int bytenr = bitnr / 8;
		int bit = bitnr % 8;
		if ((outdata[0][bytenr] & (1 << bit)) == 0)
			digitalWrite(OUT1,LOW);
		if ((outdata[1][bytenr] & (1 << bit)) == 0)
			digitalWrite(OUT2,LOW);
		if ((outdata[2][bytenr] & (1 << bit)) == 0)
			digitalWrite(OUT3,LOW);
		break;
	}
	case 2:
		digitalWrite(OUT1,LOW);
		digitalWrite(OUT2,LOW);
		digitalWrite(OUT3,LOW);
		break;
	}
}


static void IHCAddBits(int ch,word value, int bits) {

	word valuemask = 0x01 << (bits - 1);
	int i = bitpos / 8;
	int bitofs = bitpos % 8;
	uint8_t mask = 1 << bitofs;
	do {
		if (value & valuemask) {
			outdata[ch][i] |= mask;;
		}
		bitpos++;
		if ((mask <<= 1) == 0) {
			i++;
			mask = 1;
		}
	} while (valuemask >>= 1);
}
