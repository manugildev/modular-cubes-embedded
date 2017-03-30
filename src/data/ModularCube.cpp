#include <data/ModularCube.h>

#include <ESP8266WiFi.h>
#include <components/MCAccelerometer/MCAccelerometer.h>
#include <components/MCMidi/MCMidi.h>
#include <components/MCOTA/MCOTA.h>
#include <components/MCServer/MCServer.h>
#include <components/MCUDP/MCUDP.h>
#include <components/MCWiFi/MCWiFi.h>
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
  MC_WiFi.setup();
  MC_UDP.setup();
  MC_Server.setup();
  MC_OTA.setup();
  Serial.println("SetUp for ModularCube done.\n");
}

void ModularCube::loop() {
  MC_WiFi.loop();
  MC_UDP.loop();
  MC_OTA.loop();
  MC_Server.loop();
  // TODO: Change the time variable to run every X seconds
  if ((millis() - t0) > 1000) {
    t0 = millis();
    currentOrientation = MC_Accelerometer.getCurrentOrientation();
    Serial.println("Current Orientation: " + String(currentOrientation));
    // Serial.println(String(getJson()));
    if (!Cube.isMaster()) {
      String msg = String(getAPName() + " - CO:" + getCurrentOrientation());
      if (!MC_UDP.sendPacket(IPAddress(192, 168, 4, 1), msg.c_str())) {
        Serial.println("Error sending the package");
      }
    }
  }
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
  // return "{\"" + getDeviceId() + "\":{\"wlan\":\"" + getWlan() +
  //        "\",\"localIP\":\"" + getLocalIP() + "\",\"APName\":\"" +
  //        getAPName() +
  //        "\",\"currentOrientation\":\"" + getCurrentOrientation() +
  //        "\",\"connectionMode\":\"" + getConnectionMode() +
  //        "\",\"master\":\"" +
  //        isMaster() + "\",\"deviceId\":\"" + getDeviceId() + "\",\"childs\":"
  //        +
  //        getChilds() + "}}";

  return "{\"" + getDeviceId() + "\":{\"currentOrientation\":\"" +
         getCurrentOrientation() + "\",\"childs\":" + getChilds() + "}}";
}

ModularCube Cube;
