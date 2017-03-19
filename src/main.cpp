#include <Arduino.h>
#include <Wire.h>

// Custom
#include <Server.h>
#include <SimpleFirebase.h>
#include <components/Midi/MidiHelper.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

bool isConnected = false;

void setup() {
  Cube.setup();
  Midi.setup();
}
void loop() {
  Cube.loop();
  Midi.loop();
}
