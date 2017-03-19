#include <Arduino.h>
#include <Wire.h>

// Custom
#include <Server.h>
#include <SimpleFirebase.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void setup() {
  Serial.begin(115200);
  Cube.setup();
}

void loop() {
  Cube.loop();
  delay(1000);
}
