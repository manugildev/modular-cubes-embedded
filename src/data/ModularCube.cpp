#include <data/ModularCube.h>

#include <components/GameLogic/GameLogic.h>
#include <components/MCAccelerometer/MCAccelerometer.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCUDP/MCUDP.h>
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
  MC_Accelerometer.setup();
  MC_UDP.setup();

  // GL.setup();
  Serial.println("SetUp for ModularCube done.\n");
  /*if (isMaster())
    GL.switchOnLight(3000);*/
}

/****************************************************************************
* Loop
****************************************************************************/
long rNumber = random(3000, 10000);
void ModularCube::loop() {
  ledLoop();
  MC_UDP.loop();

  MC_Mesh.loop();
  // GL.loop();
  if ((millis() - t0) > 100 /*&& isActivated()*/) {
    t0 = millis();
    // This will update only every 100ms
    MC_Accelerometer.loop();
  }
  if (Cube.isActivated()) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
  }
}

void ModularCube::updateOrientation() {
  currentOrientation = MC_Accelerometer.getCurrentOrientation();
  // currentOrientation = millis();
  Serial.print("--> UpdateOrientation() - Orientation: ");
  Serial.println(currentOrientation);
  if (!Cube.isMaster()) {
    MC_Mesh.publishToAll(getJson());
  } else {
    MC_UDP.sendPacket(MC_UDP.androidIP, 3, getJsonNoChilds().c_str(),
                      MC_UDP.androidPort);
  }
}

/****************************************************************************
* METHODS
****************************************************************************/
void ModularCube::ledLoop() {}
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
void ModularCube::setActivated(bool a) {
  activated = a;
  if (!Cube.isMaster()) {
    MC_Mesh.publishToAll(getJson());
  } else {
    MC_UDP.sendPacket(MC_UDP.androidIP, 3, getJsonNoChilds().c_str(),
                      MC_UDP.androidPort);
  }
}

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
  // With the new system, i dont need to send the childs to the master
  if (isMaster()) {
    return "{\"" + String(MC_Mesh.mesh.getNodeId()) + "\":{\"" + CO_STRING +
           "\":" + getCurrentOrientation() + ",\"" + AC_STRING + "\":" +
           isActivated() + ",\"" + CH_STRING + "\":" + getChilds() + "}}";
  } else {
    return "{\"" + String(MC_Mesh.mesh.getNodeId()) + "\":{\"" + CO_STRING +
           "\":" + getCurrentOrientation() + ",\"" + AC_STRING + "\":" +
           isActivated() + "}}";
  }
}
String ModularCube::getJsonNoChilds() {
  // With the new system, i dont need to send the childs to the master
  return "{\"" + String(MC_Mesh.mesh.getNodeId()) + "\":{\"" + CO_STRING +
         "\":" + getCurrentOrientation() + ",\"" + AC_STRING + "\":" +
         isActivated() + "}}";
}
ModularCube Cube;
