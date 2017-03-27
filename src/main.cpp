#include <ESP8266HTTPClient.h>
#include <Server.h>
#include <SimpleFirebase.h>
#include <components/Midi/MidiHelper.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

int maxNumber = 0;
String previous = "0000";
void setup() { Cube.setup(); }

void loop() { Cube.loop(); }
