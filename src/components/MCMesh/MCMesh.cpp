#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

painlessMesh mesh;

void MCMesh::setup() {
  t0 = millis();
  setMasterIfMeshDoesNotExist();
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  setUpCallbacks();
  Cube.setDeviceID(mesh.getNodeId());
}

void MCMesh::loop() { mesh.update(); }

void MCMesh::setUpCallbacks() {
  mesh.onNewConnection([](uint32_t nodeId) {
    // Serial.printf("  MCMesh -> New Connection, nodeId = %u\n", nodeId);
  });
  mesh.onReceive([&](uint32_t from, String &msg) {
    // Serial.printf("  MCMesh -> New Message, nodeId = %u, msg = %s\n", from,
    // msg.c_str());
    if (Cube.isMaster()) {
      parseJsonChilds(msg);
    } else {
      parseIncomingPacket(from, msg);
    }
  });
}

void MCMesh::publish(uint32_t destId, String msg) {
  mesh.sendSingle(destId, msg);
}

void MCMesh::publishToAll(String msg) { mesh.sendBroadcast(msg); }

void MCMesh::setMasterIfMeshDoesNotExist() {
  if (checkIfMeshExists(MESH_PREFIX)) {
    // Serial.println("MCMesh -> Mesh FOUND");
    Cube.setMaster(false);
  } else {
    // Serial.println("MCMesh -> Mesh NOT FOUND");
    Cube.setMaster(true);
  }
}

bool MCMesh::checkIfMeshExists(const char *ssid, int wait) {
  // Serial.printf("Trying to find %s...\n", ssid);
  int tries = 0;
  int maxTries = (wait / 10) + 1;
  while (tries < maxTries) {
    int n = WiFi.scanNetworks();
    // Serial.printf("%i network(s) found. ", n);
    for (int i = 0; i < n; i++) {
      if (WiFi.SSID(i) == ssid) {
        return true;
      }
    }
    delay(10);
    tries++;
  }
  return false;
}

bool MCMesh::parseJsonChilds(String data) {
  DynamicJsonBuffer jsonBuffer;
  String cubeJson = Cube.getJson();
  JsonObject &root = jsonBuffer.parseObject(cubeJson);
  JsonObject &element = root[String(Cube.getDeviceId())].as<JsonObject>();
  String childs = Cube.getChilds();
  JsonObject &childsObject = element[CH_STRING].as<JsonObject>();
  JsonObject &receivedData = jsonBuffer.parseObject(data);

  if (!receivedData.success()) {
    // Serial.println("Error: MCUDP::savePacketToJson, couldn't parse the
    // Json");
    return false;
  }
  // Update if the value does not exist
  String deviceName = receivedData.begin()->key;
  JsonObject &deviceData = receivedData[deviceName].as<JsonObject>();
  childsObject[deviceName] = deviceData;
  String childString;
  childsObject.printTo(childString);
  Cube.setChilds(childString);
  MC_MQTT.publish(MQTT_TOPIC_DATA, Cube.getJson());
  return true;
}

bool MCMesh::parseIncomingPacket(uint32_t master, String data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  if (root.success()) {
    String lIP = root[LI_STRING];
    if (lIP == String(Cube.getDeviceId())) {
      int activated = root[AC_STRING].as<int>();
      Cube.setActivated(root[AC_STRING] ? true : false);
      String msg = Cube.getJson();
      // TODO: Send the master that the activate has changed
      publish(master, msg);
    }
  } else {
    // Serial.println("  MCMesh::parseIncomingPacket, parsing Json failed.");
    return false;
  }
  return true;
}

MCMesh MC_Mesh;
