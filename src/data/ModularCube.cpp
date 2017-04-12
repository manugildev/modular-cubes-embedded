#include <data/ModularCube.h>

#include <components/MCAccelerometer/MCAccelerometer.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCUDP/MCUDP.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>

/****************************************************************************
* Constructor
****************************************************************************/
ModularCube::ModularCube() {
  t0 = millis();
  setDeviceID(0);
  setWlan("");
  setLocalIP("");
  setAPName("");
  setCurrentOrientation(0);
  setMaster(false);
  setChilds("{}");
  setActivated(true);
}

/****************************************************************************
* Setup
****************************************************************************/
void ModularCube::setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println("\nSetting Up ModularCube.");
  MC_Mesh.setup();
  if (isMaster()) {
    MC_UDP.setup();
  }
  Serial.println("SetUp for ModularCube done.\n");
}

/****************************************************************************
* Loop
****************************************************************************/
long rNumber = random(3000, 10000);
void ModularCube::loop() {
  ledLoop();
  if (isMaster()) {
    MC_UDP.loop();
  }
  MC_Mesh.loop();
  if ((millis() - t0) > rNumber && isActivated()) {
    t0 = millis();
    rNumber = random(3000, 10000);
    currentOrientation = MC_Accelerometer.getCurrentOrientation();
    if (!Cube.isMaster()) {
      MC_Mesh.publishToAll(getJson());
    } else {
      String msg = "data=" + getJson();
      MC_UDP.sendPacket(MC_UDP.androidIP, msg.c_str(), MC_UDP.androidPort);
      // MC_Mesh.publishToAll(msg);
    }
  }
}

/****************************************************************************
* METHODS
****************************************************************************/
void ModularCube::ledLoop() {
  if (isActivated())
    digitalWrite(2, LOW);
  else
    digitalWrite(2, HIGH);
}
void ModularCube::reboot() {
  Serial.println("Rebooting...");
  ESP.restart();
}

/****************************************************************************
* SETTERS
****************************************************************************/
void ModularCube::setDeviceID(int dI) { deviceId = dI; }
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
int ModularCube::getDeviceId() { return deviceId; }
String ModularCube::getWlan() { return wlan; }
String ModularCube::getLocalIP() { return localIP; }
String ModularCube::getAPName() { return APName; }
String ModularCube::getChilds() { return childs; }
WiFiMode ModularCube::getConnectionMode() { return connectionMode; }
int ModularCube::getCurrentOrientation() { return currentOrientation; }
bool ModularCube::isMaster() { return master; }
bool ModularCube::isActivated() { return activated; }
String ModularCube::getJson() {
  if (isMaster()) {
    return "{\"" + String(getDeviceId()) + "\":{\"" + CO_STRING + "\":" +
           getCurrentOrientation() + ",\"" + AC_STRING + "\":" + isActivated() +
           ",\"" + CH_STRING + "\":" + getChilds() + "}}";
  } else {
    return "{\"" + String(getDeviceId()) + "\":{\"" + CO_STRING + "\":" +
           getCurrentOrientation() + ",\"" + AC_STRING + "\":" + isActivated() +
           "}}";
  }
}

ModularCube Cube;
