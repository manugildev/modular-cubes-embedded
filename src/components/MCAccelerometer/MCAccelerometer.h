#include <Arduino.h>

class MCAccelerometer {
public:
  void setup();
  void loop();
  // Variables
  int getCurrentOrientation();
  int previousOrientation;
  int currentOrientation;
};

extern MCAccelerometer MC_Accelerometer;
