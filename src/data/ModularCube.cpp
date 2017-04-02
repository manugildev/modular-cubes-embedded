#include <data/ModularCube.h>

#include <ESP8266WiFi.h>
#include <components/MCAccelerometer/MCAccelerometer.h>
#include <components/MCMQTT/MCMQTT.h>
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
  setActivated(true);
}

void ModularCube::setup() {
  Serial.begin(115200);
  Serial.println("\nSetting Up ModularCube.");
  MC_WiFi.setup();
  MC_UDP.setup();
  MC_OTA.setup();

  if (isMaster()) {
    MC_Server.setup();
    MC_MQTT.setup();
  }
  Serial.println("SetUp for ModularCube done.\n");
}

long rNumber = random(3000, 10000);
void ModularCube::loop() {

  if (isActivated())
    digitalWrite(2, LOW);
  else
    digitalWrite(2, HIGH);

  MC_WiFi.loop();
  MC_UDP.loop();
  MC_OTA.loop();
  if (isMaster()) {
    MC_Server.loop();
    MC_MQTT.loop();
  }
  if ((millis() - t0) > rNumber && isActivated()) {
    t0 = millis();
    rNumber = random(3000, 10000);
    currentOrientation = MC_Accelerometer.getCurrentOrientation();
    if (!Cube.isMaster()) {
      String msg = getFJson();
      // TODO: Send only if the previous state changes
      if (!MC_UDP.sendPacket(IPAddress(192, 168, 4, 1), msg.c_str())) {
        Serial.println("Error sending the package");
      }
    } else {
      MC_MQTT.publish(getJson());
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
void ModularCube::setChilds(String c) { childs = c; }
void ModularCube::setConnectionMode(WiFiMode wM) { connectionMode = wM; }
void ModularCube::setCurrentOrientation(int cO) { currentOrientation = cO; }
void ModularCube::setMaster(bool m) { master = m; }
void ModularCube::setActivated(bool a) { activated = a; }

/****************************************************************************
* GETTERS
****************************************************************************/
String ModularCube::getDeviceId() { return deviceId; }
String ModularCube::getWlan() { return wlan; }
String ModularCube::getLocalIP() { return localIP; }
String ModularCube::getAPName() { return APName; }
String ModularCube::getChilds() { return childs; }
WiFiMode ModularCube::getConnectionMode() { return connectionMode; }
int ModularCube::getCurrentOrientation() { return currentOrientation; }
bool ModularCube::isMaster() { return master; }
bool ModularCube::isActivated() { return activated; }
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
  if (isMaster()) {
    return "\"{\"" + getLocalIP() + "\": {\"" + DI_STRING + "\":" +
           getDeviceId() + ",\"" + CO_STRING + "\":" + getCurrentOrientation() +
           ", \"" + AC_STRING + "\":" + isActivated() + ", \"" + CH_STRING +
           "\": " + getChilds() + "}}\"";
  } else {
    return "\"{\"" + getLocalIP() + "\":{\"" + DI_STRING + "\":" +
           getDeviceId() + ", \"" + CO_STRING + "\":" +
           getCurrentOrientation() + ", \"" + AC_STRING + "\":" +
           isActivated() + "}}\"";
  }
}

String ModularCube::getFJson() {
  return getJson().substring(1, getJson().length() - 1);
}

ModularCube Cube;
