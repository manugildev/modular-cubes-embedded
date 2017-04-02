
#include <ArduinoJson.h>
#include <components/MCMQTT/MCMQTT.h>
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

bool MCUDP::sendPacket(const IPAddress &address, const char *msg,
                       uint16_t port) {
  udp.beginPacket(address, port);
  udp.write(msg);
  return (udp.endPacket() == 1);
}

bool MCUDP::receivePacket() {
  int len = udp.read(incomingPacket, PACKET_MAX_SIZE);
  if (len > 0) {
    incomingPacket[len] = 0;
  }

  if (String(incomingPacket).length() != 0) {
    Serial.printf("  MCUDP -> New Message: %s\n", incomingPacket);
    if (Cube.isMaster()) {
      sendPacket(udp.remoteIP(), replyPacket, udp.remotePort());
      parseJsonChilds(String(incomingPacket));
    } else {
      parseIncomingPacket(String(incomingPacket));
    }
    return true;
  }
  return false;
}

bool MCUDP::parseIncomingPacket(String data) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(data);
  if (root.success()) {
    String deviceId = root[DI_STRING];
    if (deviceId == Cube.getDeviceId()) {
      int activated = root[AC_STRING].as<int>();
      Cube.setActivated(root[AC_STRING] ? true : false);
    }
  } else {
    Serial.println("  MCUDP::parseIncomingPacket, parsing Json failed.");
    return false;
  }
  return true;
}

bool MCUDP::parseJsonChilds(String data) {
  DynamicJsonBuffer jsonBuffer;
  String cubeJson = Cube.getFJson();
  JsonObject &root = jsonBuffer.parseObject(cubeJson);
  JsonObject &element = root[Cube.getLocalIP()].as<JsonObject>();
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
  MC_MQTT.publish(Cube.getJson());
  return true;
}

MCUDP MC_UDP;
