#include "gui.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ A3);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 4, /* data=*/ 5, /* reset=*/ U8X8_PIN_NONE);

Gui::Gui(){

}

void Gui::init(){
  u8g2.begin();
  u8g2.enableUTF8Print();
  setSSD1306VcomDeselect(0);
  u8g2.setContrast(0);
}

void Gui::update (const Sensordata data) {
  String roomtemp = String(data.roomTemp,1) + "°";
  roomtemp.replace('.',',');
  String hum = String(data.humidity,0) + "%";
  String sp = String(data.setpointTemp,1) + "°";
  sp.replace('.',',');
  String floor = String(data.floorTemp,1) + "°";
  floor.replace('.',',');
  String lx = String(data.lux,0) + "lx";
  lx.replace('.',',');
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_helvR24_tf);  // choose a suitable font

  int wd = u8g2.getUTF8Width(roomtemp.c_str());
  u8g2.setCursor(64-wd/2, 44);
  u8g2.print(roomtemp);

  u8g2.setBitmapMode(0);
  u8g2.setDrawColor(1);
  if(data.heating)
    u8g2.drawXBMP(128 - heating_width, (64-heating_height)/2, heating_width, heating_height, heating_bits);

  u8g2.drawXBMP( 0, 49, hum_width, hum_height, hum_bits);
  u8g2.setFont(u8g2_font_helvR12_tf);  // choose a suitable font
  u8g2.setCursor(hum_width, 63);
  u8g2.print(hum.c_str());

  wd = u8g2.getUTF8Width(lx.c_str());
  u8g2.drawXBMP( 128-2-wd-lux_width, 49, lux_width, lux_height, lux_bits);
  u8g2.setCursor(128-wd, 63);
  u8g2.print(lx.c_str());

  u8g2.drawXBMP( 0, 0, setpoint_width, setpoint_height, setpoint_bits);
  u8g2.setCursor(setpoint_width, 13);
  u8g2.print(sp.c_str());

  wd = u8g2.getUTF8Width(floor.c_str());
  u8g2.drawXBMP(128-2-wd-floor_width, 0, floor_width, floor_height, floor_bits);
  u8g2.setCursor(128-wd, 13);
  u8g2.print(floor.c_str());
  u8g2.sendBuffer();
  return;
}

void Gui::setSSD1306VcomDeselect(uint8_t v)
{
  u8x8_cad_StartTransfer(u8g2.getU8x8());
  u8x8_cad_SendCmd(u8g2.getU8x8(), 0x0db);
  u8x8_cad_SendArg(u8g2.getU8x8(), v << 4);
  u8x8_cad_EndTransfer(u8g2.getU8x8());
}
