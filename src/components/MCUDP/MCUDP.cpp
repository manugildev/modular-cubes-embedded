
#include <components/MCUDP/MCUDP.h>
#include <data/ModularCube.h>

WiFiUDP udp;
#define PACKET_MAX_SIZE 255
const uint16_t localPort = 8266;
char incomingPacket[PACKET_MAX_SIZE]; // buffer for incoming packets
char replyPacket[PACKET_MAX_SIZE] = "  Message delivered.";

void MCUDP::setup() {
  if (!startUdpServer()) {
    // TODO: Take this to the modular cubes
    Serial.println("Rebooting...");
    ESP.restart();
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

// TODO: Set the port here if it is not defined!
bool MCUDP::sendPacket(const IPAddress &address, const char *msg,
                       uint16_t port) {
  udp.beginPacket(address, localPort);
  udp.write(msg);
  return (udp.endPacket() == 1);
}

bool MCUDP::receivePacket() {
  // receive incoming UDP packets
  // Serial.printf("Received %d bytes from %s, port %d\n", PACKET_MAX_SIZE,
  //               udp.remoteIP().toString().c_str(), udp.remotePort());
  int len = udp.read(incomingPacket, PACKET_MAX_SIZE);
  if (len > 0) {
    incomingPacket[len] = 0;
  }
  Serial.printf("  %s\n", incomingPacket);

  if (Cube.isMaster())
    sendPacket(udp.remoteIP(), replyPacket, udp.remotePort());
}

MCUDP MC_UDP;
