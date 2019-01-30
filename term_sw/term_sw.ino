#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ClosedCube_OPT3001.h>
#include <ClosedCube_HDC1080.h>
#include <PinChangeInterrupt.h>
#include "sensordata.h"
#include "gui.h"
#include "IHCsend.h"

volatile unsigned long pauseTimer;
volatile unsigned long pulseTimer;
volatile int receiveBuffer = 0;
volatile int8_t bitcnt = 11;

int  resolution = 12;
unsigned long lastTempRequest = 0;
int  delayInMillis = 0;

Sensordata data;
Gui display;
ClosedCube_OPT3001 opt3001;
ClosedCube_HDC1080 hdc1080;
OneWire oneWire(7);
DallasTemperature ds18b20(&oneWire);


#define BTN_LEFT 8
#define BTN_RIGHT 9

#define IN1 10

#define OPT3001_ADDRESS 0x44
#define HDC1080_ADDRESS 0x40

#define SETPOINTMAX 30
#define SETPOINTMIN 5


void setup() {
  display.init();
  data.lux = 30;
  data.roomTemp = 23.2;
  data.setpointTemp = 19.1; //External controller should check on this value to detect reset
  data.floorTemp = 34.3;
  data.heating = false;
  data.humidity = 66;
  IHCinit();
  IHCsetData(data);
  pinMode(IN1, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT);
  pinMode(BTN_RIGHT, INPUT);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BTN_LEFT), down_btn, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BTN_RIGHT), up_btn, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(IN1), input, CHANGE);
  opt3001.begin(OPT3001_ADDRESS);
  configureOPT3001();
  hdc1080.begin(HDC1080_ADDRESS);
  hdc1080.setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  
  ds18b20.begin();
  DeviceAddress tempDeviceAddress;
  ds18b20.getAddress(tempDeviceAddress, 0);
  ds18b20.setResolution(tempDeviceAddress, resolution);
  ds18b20.setWaitForConversion(false);
  ds18b20.requestTemperatures();
  delayInMillis = ds18b20.millisToWaitForConversion(resolution);
  lastTempRequest = millis();   
}

void loop() {
  if (ds18b20.isConversionComplete() || millis() - lastTempRequest >= delayInMillis) // waited long enough??
  //if(ds18b20.isConversionComplete())
  {
    data.floorTemp=ds18b20.getTempCByIndex(0);
    ds18b20.requestTemperatures();
    lastTempRequest = millis(); 
    display.update(data);
  }
    
  data.roomTemp = hdc1080.readTemperature();
  data.humidity = hdc1080.readHumidity();
  data.lux=opt3001.readResult().lux;
  
  IHCsetData(data);
}

void configureOPT3001() {
  OPT3001_Config newConfig;

  newConfig.RangeNumber = B1100;
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;

  /*OPT3001_ErrorCode errorConfig = */ opt3001.writeConfig(newConfig);
  OPT3001_Config sensorConfig = opt3001.readConfig();

}

void up_btn(){
  data.setpointTemp = int(data.setpointTemp*2.+0.5)/2. + 0.5; //round to nearest half degree
  if(data.setpointTemp > SETPOINTMAX)
    data.setpointTemp = SETPOINTMAX;
}
void down_btn(){
  data.setpointTemp = int(data.setpointTemp*2.+0.5)/2. - 0.5;//round to nearest half degree
  if(data.setpointTemp < SETPOINTMIN)
    data.setpointTemp = SETPOINTMIN;
}

void input(){
  if(digitalRead(IN1) == LOW){
    pulseTimer = millis();
    if(millis() - pauseTimer > 500){
      receiveBuffer = 0;
      bitcnt = 11;
    }
  }
  else{
    pauseTimer = millis();
    if(millis() - pulseTimer < 150)
      bitClear(receiveBuffer,bitcnt--);
    else if(millis() - pulseTimer < 250)
      bitSet(receiveBuffer,bitcnt--);
    else{
      receiveBuffer = 0;
      bitcnt = 11;
    }
    if(bitcnt<0){
      data.heating = bitRead(receiveBuffer,11);
      data.setpointTemp = float(receiveBuffer&0x3ff)/10.;
      if(bitRead(receiveBuffer,10)) data.setpointTemp *=-1;
    }
  } 
}
