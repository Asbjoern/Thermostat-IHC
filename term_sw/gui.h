#ifndef GUI_H
#define GUI_H
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "lux.h"
#include "hum.h"
#include "setpoint.h"
#include "floor.h"
#include "heating.h"
#include "sensordata.h"
class Gui {
private:
  void setSSD1306VcomDeselect(uint8_t v);
public:
  Gui();
  void init();
  void update(const Sensordata);
};
#endif
