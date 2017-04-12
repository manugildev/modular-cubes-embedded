#include <Arduino.h>

class MCAndroid {
public:
  void setup();
  void loop();
  String getResponse();
};

extern MCAndroid MC_Android;
