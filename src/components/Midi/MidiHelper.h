#include <Arduino.h>

class MidiHelper {
public:
  // Constructor
  MidiHelper();
  // Variables
  bool isConnected;
  // Methods
  void setup();
  void loop();

private:
  // Variables
  unsigned int t0; // Time manager
  // Methods
  void setupCallbacks();
};

extern MidiHelper Midi;
