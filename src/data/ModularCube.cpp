#include <components/Midi/MidiHelper.h>
#include <components/WiFi/WiFiHelper.h>
#include <components/accelerometer/Accelerometer.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void ModularCube::setup() {

  Serial.begin(115200);
  Serial.println("\nSetting Up ModularCube.");

  if (!WH.checkIfWiFiExists(configuration.cubes_ssid)) {
    WH.createWiFiAP(configuration.cubes_ssid, configuration.cubes_pass);
    // At the end it tries to connect to the home wifi
    // TODO: Setup a webserver if it cant connect to it and set the password in
    // there
    WH.connectToWiFi(configuration.ssid, configuration.pass);
  } else {
    WH.connectToWiFi(configuration.cubes_ssid, configuration.cubes_pass);
  }

  Serial.println("SetUp for ModularCube done.\n");
}

void ModularCube::loop() {
  currentOrientation = accelerometer.getCurrentOrientation();
  Serial.println(String(currentOrientation));
}

ModularCube Cube;
