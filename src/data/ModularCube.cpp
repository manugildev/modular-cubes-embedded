#include <components/WiFi/WiFiHelper.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void ModularCube::setup() {
  Serial.println("\nSetting Up ModularCube.");

  if (!WH.checkIfWiFiExists(configuration.cubes_ssid)) {
    WH.createWiFiAP(configuration.cubes_ssid, configuration.cubes_pass);
    WH.connectToWiFi(configuration.ssid, configuration.pass);
  } else {
    WH.connectToWiFi(configuration.cubes_ssid, configuration.cubes_pass);
  }
  // At the end it tries to connect to the home wifi
  // TODO: Setup a webserver if it cant connect to it and set the password in
  // there

  Serial.println("SetUp for ModularCube done.\n");
}

void ModularCube::loop() {
  currentOrientation = accelerometer.getCurrentOrientation();
  Serial.println(String(currentOrientation));
}

ModularCube Cube;
