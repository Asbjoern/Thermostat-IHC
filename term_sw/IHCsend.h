#ifndef _ihcsend_h
#define _ihcsend_h
#include "sensordata.h"

#define OUT1 5
#define OUT2 4
#define OUT3 3


static uint8_t outdata[3][9];
static int bitpos;
static unsigned long starttime;

static int val1[3]; //rum x10 / rum x10 / rum x10
static int val2[3]; //setpunkt x10 / ?? / rh x10
static int val3[3]; //gulv x10 / lux / dugpunkt x10

static void IHCfillBuffer();
static void IHCAddBits(int ch, word value, int bits = 12);

void IHCinit();
static void IHCTick(void);

void IHCsetData(Sensordata &data);
	
#endif
