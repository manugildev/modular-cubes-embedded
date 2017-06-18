
#include <ArduinoJson.h>
#include <components/GameLogic/GameLogic.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <components/MCUDP/MCUDP.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

WiFiUDP udp;
#define PACKET_MAX_SIZE 255
const uint16_t localPort = 8266;
char incomingPacket[PACKET_MAX_SIZE]; // buffer for incoming packets
char replyPacket[PACKET_MAX_SIZE] = "{\"message\": \"delivered\"}";

void MCUDP::setup() {
  if (!startUdpServer()) {
    Cube.reboot();
  }
}

void MCUDP::loop() {
  if (udp.parsePacket()) {
    receivePacket();
  }
}

bool MCUDP::startUdpServer() {
  if (!udp.begin(localPort)) {
    Serial.println("Error starting UDP server!");
    return false;
  }
  Serial.print("UDP server started on port ");
  Serial.println(udp.localPort());
  return true;
}

bool MCUDP::sendPacket(const IPAddress &address, int messageType,
                       char const *msg, uint16_t port) {
  udp.beginPacket(address, port);
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
  }
  udp.write(st.c_str());
  return (udp.endPacket() == 1);
}

bool MCUDP::receivePacket() {
  int len = udp.read(incomingPacket, PACKET_MAX_SIZE);
  if (len > 0) {
    incomingPacket[len] = 0;
  }

  if (String(incomingPacket).length() != 0) {
    String ip = udp.remoteIP().toString() + ":" + udp.remotePort();
    Serial.printf("  MCUDP -> New Message: %s, from %s\n", incomingPacket,
                  ip.c_str());
    if (String(incomingPacket).indexOf("android") != -1)
      Cube.setMaster(true);

    if (Cube.isMaster()) {
      // This is for the first message the app sends
      if (String(incomingPacket).indexOf("android") != -1) {
        parseAndroidPacket(udp.remoteIP(), udp.remotePort(),
                           String(incomingPacket));
        Serial.println("initial running");
        Serial.println(incomingPacket);
        MC_UDP.sendPacket(MC_UDP.androidIP, Initial,
                          Cube.getJsonNoChilds().c_str(), MC_UDP.androidPort);
      } else {
        parseAndroidPacket(udp.remoteIP(), udp.remotePort(),
                           String(incomingPacket));
      }
      // parseJsonChilds(String(incomingPacket));
    } else {
      // Only if master, for sure
      // sendPacket(udp.remoteIP(), replyPacket, udp.remotePort());
      // parseIncomingPacket(String(incomingPacket));
    }
    return true;
  }
  return false;
}

bool MCUDP::parseAndroidPacket(IPAddress ip, uint32_t port,
                               String incomingPacket) {
  androidIP = ip;
  androidPort = port;
  // Serial.println(incomingPacket);
  if (incomingPacket.indexOf("lIP") != -1) {
    parseActivate(incomingPacket);
  } else if (incomingPacket.indexOf("gamemode") != -1) {
    parseGameMode(incomingPacket);
  } else if (incomingPacket.indexOf("connections") != -1) {
    MC_UDP.sendPacket(MC_UDP.androidIP, 4,
                      MC_Mesh.mesh.subConnectionJson().c_str(),
                      MC_UDP.androidPort);
  } else if (incomingPacket.indexOf("all") != -1) {
    parseAllPackage(incomingPacket);
  }
  return false;
}

bool MCUDP::parseAllPackage(String response) {
  response.replace("all=", "");
  if (response.indexOf("start") != -1) {
    Cube.setActivated(true);
    MC_Mesh.publishToAll("start");
  } else if (response.indexOf("stop") != -1) {
    Cube.setActivated(false);
    MC_Mesh.publishToAll("stop");
  }
}

bool MCUDP::parseActivate(String response) {
  DynamicJsonBuffer jsonBuffer;
  JsonArray &array = jsonBuffer.parseArray(response);
  if (array.success()) {
    for (int i = 0; i < array.size(); i++) {
      String lIP = array[i][LI_STRING].as<String>();
      if (lIP.toInt() != Cube.getDeviceId()) {
        Serial.println("  MCUDP -> Send Activate to: " + lIP + " - " +
                       array[i].as<String>());
        MC_Mesh.publish(lIP.toInt(), array[i].as<String>());
      } else {
        int activated = array[i][AC_STRING].as<int>();
        if (Cube.isActivated()) {
          Cube.setActivated(false);
        } else {
          Cube.setActivated(true);
        }
      }
    }
    return true;
  } else {
    Serial.println("  MCUDP:parseData, failed parsing Json Data.");
    return false;
  }
  return false;
}

bool MCUDP::parseGameMode(String gameMode) {}

MCUDP MC_UDP;
