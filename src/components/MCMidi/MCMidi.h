#include <Arduino.h>

class MCMidi {
public:
  // Constructor
  MCMidi();
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

extern MCMidi MC_Midi;
