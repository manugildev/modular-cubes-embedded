#include <components/MCEasyMesh/MCEasyMesh.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

#define MESH_PREFIX "CUBES_MESH" // TODO: Turn this into the configuration file
#define MESH_PORT 5555
easyMesh mesh;

void MCEasyMesh::setup() {
  t0 = millis();
  setMasterIfMeshDoesNotExists();
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  // mesh.init(MESH_PREFIX, WIFI_PASSWORD, MESH_PORT);
  // if (Cube.isMaster()) // TODO: Move this to the wifi class
  //   MC_WiFi.connectToWiFi(WIFI_SSID, WIFI_PASSWORD, 10000);
  setUpCallbacks();
}

void MCEasyMesh::loop() {
  mesh.update();
  if ((millis() - t0) > 1000) {
    t0 = millis();
    String msg = "Hello from node ";
    msg += mesh.getChipId();
    mesh.sendBroadcast(msg);
  }
}

void MCEasyMesh::setUpCallbacks() {
  mesh.setReceiveCallback([](uint32_t from, String &msg) {
    Serial.printf("startHere: Received from %d mesg %s\n", from, msg.c_str());
  });
  mesh.setNewConnectionCallback([](bool adopt) {
    Serial.printf("startHere: New Connection adopt=%d\n", adopt);
  });
}

void MCEasyMesh::publish(uint32_t destId, String msg) {
  mesh.sendSingle(destId, msg);
}

void MCEasyMesh::publishToAll(String msg) { mesh.sendBroadcast(msg); }

void MCEasyMesh::setMasterIfMeshDoesNotExists() {
  if (checkIfMeshCreated(MESH_PREFIX)) {
    Serial.println("  MCEasyMesh -> Mesh FOUND");
    Cube.setMaster(false);
  } else {
    Serial.println(" MCEasyMesh -> Mesh NOT FOUND");
    Cube.setMaster(true);
  }
}

// Checks if the wifi passed in the parameters exist, returns true or false
bool MCEasyMesh::checkIfMeshCreated(const char *prefix, int wait) {
  Serial.printf("Trying to find %s...\n", prefix);
  int tries = 0;
  int maxTries = (wait / 10) + 1;
  // We set the WiFi to Station Mode and Disconnect from any other WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (tries < maxTries) {
    int n = WiFi.scanNetworks();
    Serial.printf("%i - %i network(s) found. ", tries, n);
    for (int i = 0; i < n; i++) {
      if (WiFi.SSID(i).startsWith(prefix)) {
        return true;
      }
    }
    Serial.printf("%s Not found.\n", prefix);
    delay(10);
    tries++;
  }
  Serial.printf("After %i tries %s could not be found.\n", tries, prefix);
  return false;
}

MCEasyMesh MC_EasyMesh;
