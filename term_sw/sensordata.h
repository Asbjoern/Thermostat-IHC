#ifndef SENSORDATA_H
#define SENSORDATA_H
class Sensordata {
   public:
      float roomTemp;
      float setpointTemp;
      float floorTemp;
      float humidity;
      unsigned int lux;
      bool heating;
};
#endif
