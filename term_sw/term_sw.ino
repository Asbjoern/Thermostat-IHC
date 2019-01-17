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
  data.lux = 30.3;
  data.roomTemp = 23.2;
  data.setpointTemp = 22.0;
  data.floorTemp = 34.3;
  data.heating = false;
  data.humidity = 66;
  IHCinit();
  IHCsetData(data);
  pinMode(BTN_LEFT, INPUT);
  pinMode(BTN_RIGHT, INPUT);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BTN_LEFT), down_btn, RISING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(BTN_RIGHT), up_btn, RISING);
  opt3001.begin(OPT3001_ADDRESS);
  hdc1080.begin(HDC1080_ADDRESS);
  ds18b20.begin();
  hdc1080.setResolution(HDC1080_RESOLUTION_14BIT, HDC1080_RESOLUTION_14BIT);
  configureOPT3001();
}

void loop() {
  ds18b20.requestTemperatures();
  data.roomTemp = hdc1080.readTemperature();
  data.humidity = hdc1080.readHumidity();
  data.lux=opt3001.readResult().lux;
  data.floorTemp=ds18b20.getTempCByIndex(0);
  display.update(data);
  IHCsetData(data);
 //delay(1000);

}

void configureOPT3001() {
  OPT3001_Config newConfig;

  newConfig.RangeNumber = B1100;
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;

  /*OPT3001_ErrorCode errorConfig = */ opt3001.writeConfig(newConfig);
  //OPT3001_Config sensorConfig = opt3001.readConfig();

}

void up_btn(){
  if(data.setpointTemp <= SETPOINTMAX)
    data.setpointTemp +=0.5;
}
void down_btn(){
  if(data.setpointTemp >= SETPOINTMIN)
    data.setpointTemp -=0.5;
}
