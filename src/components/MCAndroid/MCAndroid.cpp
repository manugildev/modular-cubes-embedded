
#include <ArduinoJson.h>
#include <components/MCAndroid/MCAndroid.h>
#include <components/MCGameLogic/MCGameLogic.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

WiFiUDP android;
#define PACKET_MAX_SIZE 255
const uint16_t localPort = 8266;
char incomingPacket[PACKET_MAX_SIZE]; // buffer for incoming packets
char replyPacket[PACKET_MAX_SIZE] = "all_nodes=confirmed";

void MCAndroid::setup() {
  if (!startUdpServer()) {
    Cube.reboot();
  }
}

void MCAndroid::loop() {
  if (android.parsePacket()) {
    receivePacket();
  }
}

bool MCAndroid::startUdpServer() {
  if (!android.begin(localPort)) {
    Serial.println("Error starting TCP server!");
    return false;
  }
  Serial.print("TCP server started on port ");
  Serial.println(android.localPort());
  return true;
}

bool MCAndroid::sendPacket(const IPAddress &address, int messageType,
                           char const *msg, uint16_t port) {
  android.beginPacket(address, port);
  String st;
  switch (messageType) {
  case Initial:
    st = "initial=" + String(msg);
    break;
  case Connection:
    st = "connection=" + String(msg);
    break;
  case Disconnection:
    st = "disconnection=" + String(msg);
    break;
  case Information:
    st = "information=" + String(msg);
    break;
  case Connections:
    st = "connections=" + String(msg);
    break;
  default:
    st = String(msg);
    break;
  }
  android.write(st.c_str());
  return (android.endPacket() == 1);
}

bool MCAndroid::receivePacket() {
  int len = android.read(incomingPacket, PACKET_MAX_SIZE);
  if (len > 0) {
    incomingPacket[len] = 0;
  }

  if (String(incomingPacket).length() != 0) {
    String ip = android.remoteIP().toString() + ":" + android.remotePort();
    Serial.printf("  MCAndroid -> New Message: %s, from %s\n", incomingPacket,
                  ip.c_str());
    if (String(incomingPacket).indexOf("android") != -1) {
      String response = "master=" + String(Cube.getDeviceId());
      MC_Mesh.publishToAll(response.c_str());
      Cube.setMaster(true);
    }

    if (Cube.isMaster()) {
      // This is for the first message the app sends
      if (String(incomingPacket).indexOf("android") != -1) {
        parseAndroidPacket(android.remoteIP(), android.remotePort(),
                           String(incomingPacket));
        Serial.println("initial running");
        Serial.println(incomingPacket);
        MC_Android.sendPacket(MC_Android.androidIP, Initial,
                              Cube.getJsonNoChilds().c_str(),
                              MC_Android.androidPort);
      } else {
        parseAndroidPacket(android.remoteIP(), android.remotePort(),
                           String(incomingPacket));
      }
      // parseJsonChilds(String(incomingPacket));
    } else {
      // Only if master, for sure
      // sendPacket(android.remoteIP(), replyPacket, android.remotePort());
      // parseIncomingPacket(String(incomingPacket));
    }
    return true;
  }
  return false;
}

bool MCAndroid::parseAndroidPacket(IPAddress ip, uint32_t port,
                                   String incomingPacket) {
  androidIP = ip;
  androidPort = port;
  // Serial.println(incomingPacket);
  if (incomingPacket.indexOf("lIP") != -1) {
    parseActivate(incomingPacket);
  } else if (incomingPacket.indexOf("gamemode") != -1) {
    parseGameMode(incomingPacket);
  } else if (incomingPacket.indexOf("connections") != -1) {
    MC_Android.sendPacket(MC_Android.androidIP, 4,
                          MC_Mesh.mesh.subConnectionJson().c_str(),
                          MC_Android.androidPort);
  } else if (incomingPacket.indexOf("all") != -1) {
    parseAllPackage(incomingPacket);
  }
  return false;
}

bool MCAndroid::parseAllPackage(String response) {
  response.replace("all=", "");
  if (response.indexOf("start") != -1) {
    Cube.setActivated(true, false);
    MC_Mesh.publishToAll("start");
  } else if (response.indexOf("stop") != -1) {
    Cube.setActivated(false, false);
    MC_Mesh.publishToAll("stop");
  }
  // Confirmation message to know everyone has received the data
  sendPacket(androidIP, 5, replyPacket, androidPort);
}

bool MCAndroid::parseActivate(String response) {
  DynamicJsonBuffer jsonBuffer;
  JsonArray &array = jsonBuffer.parseArray(response);
  if (array.success()) {
    for (int i = 0; i < array.size(); i++) {
      String lIP = array[i][LI_STRING].as<String>();
      if (lIP.toInt() != Cube.getDeviceId()) {
        Serial.println("  MCAndroid -> Send Activate to: " + lIP + " - " +
                       array[i].as<String>());
        MC_Mesh.publish(lIP.toInt(), array[i].as<String>());
      } else {
        int activated = array[i][AC_STRING].as<int>();
        int response;
        if (array[i].as<JsonObject>().containsKey(R_STRING))
          response = array[i][R_STRING].as<int>();
        else
          response = 1;
        if (Cube.isActivated()) {
          Cube.setActivated(false, response == 1 ? true : false);
        } else {
          Cube.setActivated(true, response == 1 ? true : false);
        }
      }
    }
    return true;
  } else {
    Serial.println("  MCAndroid:parseData, failed parsing Json Data.");
    return false;
  }
  return false;
}

bool MCAndroid::parseGameMode(String gameMode) {}

MCAndroid MC_Android;
