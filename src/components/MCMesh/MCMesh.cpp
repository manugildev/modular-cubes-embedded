#include <Arduino.h>
#include <components/MCAndroid/MCAndroid.h>
#include <components/MCGameLogic/MCGameLogic.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

modularMesh mesh;

void MCMesh::setup() {
  t0 = millis();
  // TODO: No need of this
  // setMasterIfMeshDoesNotExist();
  mesh.setDebugMsgTypes(ERROR /*| COMMUNICATION | MESH_STATUS | DEBUG*/);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, STA_AP, AUTH_WPA2_PSK, 1,
            PHY_MODE_11G, 82, 0, 4);

  setUpCallbacks();
  Cube.setDeviceID(mesh.getNodeId());
}

void MCMesh::loop() {
  mesh.update();
  if ((millis() - t0) > 1000 && !Cube.isMaster() && masterId == 0) {
    t0 = millis();
    publishToAll("master?");
  }
}

void MCMesh::setUpCallbacks() {
  mesh.onNewConnection([&](uint32_t nodeId) {
    Serial.printf("  MCMesh -> New Connection, nodeId = %u\n", nodeId);
    if (Cube.isMaster()) {
      MC_Android.sendPacket(MC_Android.androidIP, 1, String(nodeId).c_str(),
                            MC_Android.androidPort);
      String msg = "master=" + String(mesh.getNodeId());
      MC_Mesh.publish(nodeId, msg.c_str());
    } else {
      //String msg = "master=" + String(masterId);
      //MC_Mesh.publish(nodeId, msg.c_str());
    }
  });

  mesh.onReceive([&](uint32_t from, String &msg) {
    Serial.printf("  MCMesh -> New Message, nodeId = %u, msg = %s\n", from,
                  msg.c_str());
    if (Cube.isMaster()) {
      if (msg.indexOf("light") != -1) {
        parseIncomingPacket(from, msg);
      } else if (msg.indexOf("master?") != -1) {
        String response = "master=" + String(mesh.getNodeId());
        MC_Mesh.publish(from, response.c_str());
      } else {
        parseJsonChilds(msg);
      }
    } else {
      // Serial.println(msg);
      if (msg.indexOf("master=") != -1) {
        masterId = from;
      } else if (msg.indexOf("master?") != -1) {
        //String msg = "master=" + String(masterId);
        //MC_Mesh.publish(from, msg.c_str());
      } else if (msg.indexOf("start") != -1) {
        Cube.setActivated(true, false);
      } else if (msg.indexOf("stop") != -1) {
        Cube.setActivated(false, false);
      } else {
        parseIncomingPacket(from, msg);
      }
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
        // Serial.println(key);
        uint32_t id = strtoul(key, NULL, 10);
        const uint32_t number = id;
        // Serial.println(number);
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
          char textToWrite[16];
          sprintf(textToWrite, "%lu", *itr);
          childsObject.remove(textToWrite);
          String childString;
          childsObject.printTo(childString);
          Cube.setChilds(childString);
          MC_Android.sendPacket(MC_Android.androidIP, 2,
                                String(textToWrite).c_str(),
                                MC_Android.androidPort);
        }
        // Serial.println(Cube.getJson());
      }
    }
  });
}

void MCMesh::setMasterId(uint32_t masterId) { masterId = masterId; }

bool MCMesh::publish(uint32_t destId, String msg) {
  // Serial.println("DestID:" + String(destId) + " " + msg);
  return mesh.sendSingle(destId, msg);
}

bool MCMesh::publishToMaster(String msg) {
  // Serial.println(masterId);
  if (masterId == 0) {
    publishToAll("master?");
  } else {
    return publish(masterId, msg);
  }
}

bool MCMesh::publishToAll(String msg) {
  Serial.println("  MCMesh -> PublishToAll: " + msg);
  return mesh.sendBroadcast(msg);
}

void MCMesh::setMasterIfMeshDoesNotExist() {
  // Todo: Check if this needs to be changed
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
    Serial.println("Error: MCMesh::parseJsonChilds, couldn't parse the Json");
    return false;
  }
  // Update if the value does not exist
  String deviceName = receivedData.begin()->key;
  JsonObject &deviceData = receivedData[deviceName].as<JsonObject>();
  childsObject[deviceName] = deviceData;
  String childString;
  childsObject.printTo(childString);
  Cube.setChilds(childString);
  MC_Android.sendPacket(MC_Android.androidIP, 3, data.c_str(),
                        MC_Android.androidPort);
  return true;
}

bool MCMesh::parseIncomingPacket(uint32_t master, String data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  if (root.success()) {
    // if (data.indexOf("light") != -1) {
    //   return parseGameLight(data);
    // } else {
    return parseActivate(master, data);
    // }
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
    MC_GameLogic.switchOnLight(t);
    return true;
  }
  return false;
}

bool MCMesh::parseActivate(uint32_t master, String data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  String lIP = root[LI_STRING];
  int activated = root[AC_STRING].as<int>();
  int response;
  if (root.containsKey(R_STRING))
    response = root[R_STRING].as<int>();
  else
    response = 1;
  if (lIP.toInt() == Cube.getDeviceId()) {
    if (Cube.isActivated())
      Cube.setActivated(false, response == 1 ? true : false);
    else
      Cube.setActivated(true, response == 1 ? true : false);
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
