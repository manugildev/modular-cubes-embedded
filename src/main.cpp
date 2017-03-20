#include <Arduino.h>
#include <Wire.h>

// Custom
#include <Server.h>
#include <SimpleFirebase.h>
#include <components/Midi/MidiHelper.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void setup() { Cube.setup(); }
void loop() { Cube.loop(); }
