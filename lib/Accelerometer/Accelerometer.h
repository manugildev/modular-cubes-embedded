#include <Arduino.h>

class Accelerometer {
public:
  int getCurrentOrientation();
};

extern Accelerometer accelerometer;
