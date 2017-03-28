#include <data/ModularCube.h>

#include <ESP8266WiFi.h>
#include <components/MCAccelerometer/MCAccelerometer.h>
#include <components/MCMidi/MCMidi.h>
#include <components/MCServer/MCServer.h>
#include <components/WiFi/WiFiHelper.h>
#include <configuration/Configuration.h>

ModularCube::ModularCube() {
  t0 = millis();
  setDeviceName(""); // I could set this to the SSID
  setDeviceID(String(ESP.getChipId()));
  setWlan("");
  setLocalIP("");
  setCurrentOrientation(0);
  setMaster(false);
}

void ModularCube::setup() {
  Serial.begin(115200);
  Serial.println("\nSetting Up ModularCube.");
  setUpWiFi();
  MC_Server.begin();
  Serial.println("SetUp for ModularCube done.\n");
}

void ModularCube::loop() {
  MC_Server.loop();
  // TODO: Change the time variable to run every X seconds
  if ((millis() - t0) > 1000) {
    t0 = millis();
    currentOrientation = MC_Accelerometer.getCurrentOrientation();
    Serial.println(String(currentOrientation));
  }
}

void ModularCube::setUpWiFi() {
  String mssid = String(String(configuration.cubes_ssid) + "_M");
  String wifiName = WH.generateSSID();
  String connectTo = WH.getConnectTo(wifiName);
  WH.createWiFiAP(wifiName.c_str());
  if (wifiName == mssid) {
    WH.connectToWiFi(connectTo.c_str(), configuration.pass);
  } else {
    WH.connectToWiFi(connectTo.c_str());
  }
  setConnectionMode(WiFi.getMode());
}

/****************************************************************************
* SETTERS
****************************************************************************/
void ModularCube::setDeviceName(String dN) { deviceName = dN; }
void ModularCube::setDeviceID(String dI) { deviceId = dI; }
void ModularCube::setWlan(String w) { wlan = w; }
void ModularCube::setLocalIP(String lI) { localIP = lI; }
void ModularCube::setCurrentOrientation(int cO) { currentOrientation = cO; }
void ModularCube::setConnectionMode(WiFiMode wM) { connectionMode = wM; }

/****************************************************************************
* GETTERS
****************************************************************************/
String ModularCube::getDeviceName() { return deviceName; }
String ModularCube::getDeviceId() { return deviceId; }
String ModularCube::getWlan() { return wlan; }
String ModularCube::getLocalIP() { return localIP; }
int ModularCube::getCurrentOrientation() { return currentOrientation; }
WiFiMode ModularCube::getConnectionMode() { return connectionMode; }

String getJson() {}

ModularCube Cube;
