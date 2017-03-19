#include <Arduino.h>

class MidiHelper {
public:
  void setup();
  void loop();

private:
  bool isConnected;
};

extern MidiHelper Midi;
