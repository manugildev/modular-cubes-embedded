#include <Arduino.h>

class MCGameLogic {
public:
  void setup();
  void loop();
  void switchOnLight(int milliseconds);
  void switchRandomLightInMesh(int addSeconds);
  String createJsonLight(uint32_t randomNode, int addSeconds);

  int currentMilliseconds;

private:
  int endTime;
  int startTime;
  int gameMode = 0;
};

extern MCGameLogic MC_GameLogic;
