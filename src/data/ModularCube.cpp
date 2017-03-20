#include <components/Midi/MidiHelper.h>
#include <components/WiFi/WiFiHelper.h>
#include <components/accelerometer/Accelerometer.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

ModularCube::ModularCube() {
  t0 = millis();
  deviceName = "";
  deviceId = "";
  wlan = "";
  localIP = "";
  currentOrientation = 0;
}

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

  Midi.setup();
  Serial.println("SetUp for ModularCube done.\n");
}

void ModularCube::loop() {
  // TODO: Change the time variable to run every X seconds
  if ((millis() - t0) > 0) {
    t0 = millis();
    currentOrientation = accelerometer.getCurrentOrientation();
    Serial.println(String(currentOrientation));
  }
  Midi.loop();
}

/****************************************************************************
* SETTERS
****************************************************************************/
void ModularCube::setDeviceName(String dN) { deviceName = dN; }
void ModularCube::setDeviceID(String dI) { deviceId = dI; }
void ModularCube::setWlan(String w) { wlan = w; }
void ModularCube::setLocalIP(String lI) { localIP = lI; }
void ModularCube::setCurrentOrientation(int cO) { currentOrientation = cO; }

/****************************************************************************
* GETTERS
****************************************************************************/
String ModularCube::getDeviceName() { return deviceName; }
String ModularCube::getDeviceId() { return deviceId; }
String ModularCube::getWlan() { return wlan; }
String ModularCube::getLocalIP() { return localIP; }
int ModularCube::getCurrentOrientation() { return currentOrientation; }

ModularCube Cube;
