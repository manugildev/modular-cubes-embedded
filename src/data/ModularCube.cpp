#include <data/ModularCube.h>

#include <ESP8266WiFi.h>
#include <components/MCAccelerometer/MCAccelerometer.h>
#include <components/MCMidi/MCMidi.h>
#include <components/MCServer/MCServer.h>
#include <components/WiFi/WiFiHelper.h>
#include <configuration/Configuration.h>

ModularCube::ModularCube() {
  t0 = millis();
  setDeviceID(String(ESP.getChipId()));
  setWlan("");
  setLocalIP("");
  setAPName("");
  setCurrentOrientation(0);
  setMaster(false);
  setChilds("{}");
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
    // Serial.println(String(getJson()));
  }
}

void ModularCube::setUpWiFi() {
  String mssid = String(String(configuration.cubes_ssid) + "_M");
  String wifiName = WH.generateSSID();
  String connectTo = WH.getConnectTo(wifiName);
  WH.createWiFiAP(wifiName.c_str());
  if (wifiName == mssid) {
    setMaster(true);
    if (WH.connectToWiFi(connectTo.c_str(), configuration.pass))
      setWlan(connectTo);
  } else {
    if (WH.connectToWiFi(connectTo.c_str()))
      setWlan(connectTo);
  }
  setAPName(wifiName);
  setLocalIP(String(WiFi.localIP()));
  setConnectionMode(WiFi.getMode());
}

/****************************************************************************
* SETTERS
****************************************************************************/
void ModularCube::setDeviceID(String dI) { deviceId = dI; }
void ModularCube::setWlan(String w) { wlan = w; }
void ModularCube::setLocalIP(String lI) { localIP = lI; }
void ModularCube::setAPName(String APN) { APName = APN; }
void ModularCube::setCurrentOrientation(int cO) { currentOrientation = cO; }
void ModularCube::setConnectionMode(WiFiMode wM) { connectionMode = wM; }
void ModularCube::setMaster(bool m) { master = m; }
void ModularCube::setChilds(String c) { childs = c; }

/****************************************************************************
* GETTERS
****************************************************************************/
String ModularCube::getDeviceId() { return deviceId; }
String ModularCube::getWlan() { return wlan; }
String ModularCube::getLocalIP() { return localIP; }
String ModularCube::getAPName() { return APName; }
int ModularCube::getCurrentOrientation() { return currentOrientation; }
WiFiMode ModularCube::getConnectionMode() { return connectionMode; }
bool ModularCube::isMaster() { return master; }
String ModularCube::getChilds() { return childs; }
String ModularCube::getJson() {
  // TODO: Childs should only appear maybe on the MASTER, or maybe not. Since
  // this network is based on a TREE Network
  return "{\"" + getAPName() + "\":{\"wlan\":\"" + getWlan() +
         "\",\"localIP\":\"" + getLocalIP() + "\",\"APName\":\"" + getAPName() +
         "\",\"currentOrientation\":\"" + getCurrentOrientation() +
         "\",\"connectionMode\":\"" + getConnectionMode() + "\",\"master\":\"" +
         isMaster() + "\",\"deviceId\":\"" + getDeviceId() + "\",\"childs\":" +
         getChilds() + "}}";
}

ModularCube Cube;
