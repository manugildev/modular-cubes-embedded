#include <Arduino.h>

class GameLogic {
public:
  void setup();
  void loop();
  void clicked();
  
  void switchOnLight(int milliseconds);
  void switchRandomLightInMesh(int addSeconds);
  String createJsonLight(uint32_t randomNode, int addSeconds);
  int currentMilliseconds;
private:
  int endTime;
  int startTime;
};

extern GameLogic GL;
