#include <Arduino.h>
#include <components/GameLogic/GameLogic.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCUDP/MCUDP.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

painlessMesh mesh;

void MCMesh::setup() {
  t0 = millis();
  setMasterIfMeshDoesNotExist();
  mesh.setDebugMsgTypes(ERROR);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 1,
            PHY_MODE_11G, 82, !Cube.isMaster(), 4);

  setUpCallbacks();
  Cube.setDeviceID(mesh.getNodeId());
}

void MCMesh::loop() { mesh.update(); }

void MCMesh::setUpCallbacks() {
  mesh.onNewConnection([](uint32_t nodeId) {
    Serial.printf("  MCMesh -> New Connection, nodeId = %u\n", nodeId);
  });
  mesh.onReceive([&](uint32_t from, String &msg) {
    Serial.printf("  MCMesh -> New Message, nodeId = %u, msg = %s\n", from,
                  msg.c_str());
    if (Cube.isMaster()) {
      if (msg.indexOf("light") == -1)
        parseJsonChilds(msg);
      else
        parseIncomingPacket(from, msg);
    } else {
      parseIncomingPacket(from, msg);
    }
  });
  mesh.onChangedConnections([&]() {
    if (Cube.isMaster()) {
      DynamicJsonBuffer jsonBuffer;
      String cubeJson = Cube.getJson();
      JsonObject &root = jsonBuffer.parseObject(cubeJson);
      JsonObject &element = root[String(Cube.getDeviceId())].as<JsonObject>();
      String childs = Cube.getChilds();
      JsonObject &childsObject = element[CH_STRING].as<JsonObject>();

      SimpleList<uint32_t> nodes = mesh.getNodeList();
      SimpleList<uint32_t> childList;

      // Posible fallo aqui con la numeración
      for (JsonObject::iterator it = childsObject.begin();
           it != childsObject.end(); ++it) {
        const char *key = it->key;
        Serial.println(key);
        uint32_t id = strtoul(key, NULL, 10);
        const uint32_t number = id;
        Serial.println(number);
        childList.push_back(number);
      }

      // Serial.println("ChildListSize: " + String(childList.size()) + "
      // NodesSize:" + String(nodes.size()));
      for (SimpleList<uint32_t>::iterator itr = childList.begin();
           itr != childList.end(); ++itr) {
        bool contains = false;
        for (SimpleList<uint32_t>::iterator itr1 = nodes.begin();
             itr1 != nodes.end(); ++itr1) {
          if (*itr == *itr1) {
            // Serial.println("ChildList: " + String(*itr) + " Nodes:" +
            // String(*itr1) + " Contains = true");
            contains = true;
          }
        }

        if (!contains && *itr != mesh.getNodeId()) {
          // Serial.print("ChildList: ");
          char textToWrite[16];
          sprintf(textToWrite, "%lu", *itr);
          //  Serial.print(textToWrite);
          //  Serial.println(" Contains = false");
          childsObject.remove(textToWrite);
          String childString;
          childsObject.printTo(childString);
          Cube.setChilds(childString);
          // Delete from childs
        }
        Serial.println(Cube.getJson());
      }
    }
  });
}

bool MCMesh::publish(uint32_t destId, String msg) {
  return mesh.sendSingle(destId, msg);
}

bool MCMesh::publishToAll(String msg) { return mesh.sendBroadcast(msg); }

void MCMesh::setMasterIfMeshDoesNotExist() {
  if (checkIfMeshExists(MESH_PREFIX)) {
    Serial.println("MCMesh -> Mesh FOUND");
    Cube.setMaster(false);
  } else {
    Serial.println("MCMesh -> Mesh NOT FOUND");
    Cube.setMaster(true);
  }
}

bool MCMesh::checkIfMeshExists(const char *ssid, int wait) {
  Serial.printf("Trying to find %s...\n", ssid);
  int tries = 0;
  int maxTries = (wait / 10) + 1;
  while (tries < maxTries) {
    int n = WiFi.scanNetworks();
    Serial.printf("%i network(s) found. ", n);
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
    Serial.println("Error: MCUDP::savePacketToJson, couldn't parse the Json");
    return false;
  }
  // Update if the value does not exist
  String deviceName = receivedData.begin()->key;
  JsonObject &deviceData = receivedData[deviceName].as<JsonObject>();
  childsObject[deviceName] = deviceData;
  String childString;
  childsObject.printTo(childString);
  Cube.setChilds(childString);

  String msg = "data=" + Cube.getJson();
  MC_UDP.sendPacket(MC_UDP.androidIP, msg.c_str(), MC_UDP.androidPort);
  return true;
}

bool MCMesh::parseIncomingPacket(uint32_t master, String data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  if (root.success()) {
    if (data.indexOf("light") != -1) {
      return parseGameLight(data);
    } else {
      return parseActivate(master, data);
    }
  } else {
    Serial.println("  MCMesh::parseIncomingPacket, parsing Json failed.");
    return false;
  }
  return true;
}

bool MCMesh::parseGameLight(String data) {
  // TODO: Cambiar lIP for deviceId everywhere and change the parsing thing
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  uint32_t lIP = root[LI_STRING].as<uint32_t>();
  if (lIP == Cube.getDeviceId()) {
    int light = root["light"];
    int t = root["time"].as<int>();
    GL.switchOnLight(t);
    return true;
  }
  return false;
}

bool MCMesh::parseActivate(uint32_t master, String data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  String lIP = root[LI_STRING];
  int activated = root[AC_STRING].as<int>();
  if (!activated) {
    // GL.switchRandomLightInMesh(+50);
    return true;
  }
  return false;
}
/*
* Return a random node id from the nodes list. If you have only this node it
* returns 0
*/
uint32_t MCMesh::getRandomNode() {
  SimpleList<uint32_t> nodes = mesh.getNodeList();
  int i = 0;
  int r = random(0, nodes.size());
  for (SimpleList<uint32_t>::iterator itr = nodes.begin(); itr != nodes.end();
       ++itr) {
    if (i == r)
      return *itr;
    i++;
  }
  return 0;
}

MCMesh MC_Mesh;
