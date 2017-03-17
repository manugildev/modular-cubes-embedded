#include <Arduino.h>
#include <Wire.h>

// Custom
#include <Firebase.h>
#include <Server.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void setup() {
  Cube.setup();

  firebase.begin(configuration.firebase_host, configuration.firebase_auth);
}

void loop() {
  Cube.loop();
  delay(1000);
  Serial.println("\nGET =================================");
  String getResponse = firebase.GET("/numbers");
}
