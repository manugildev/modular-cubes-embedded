#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>

class MCUDP {
public:
  void setup();
  void loop();
  bool startUdpServer();
  bool receivePacket();
  bool sendPacket(const IPAddress &address, const char *msg,
                  uint16_t port = 8266);
  bool parseIncomingPacket(String data);
  bool parseAndroidPacket(IPAddress ip, uint32_t port, String incomingPacket);
  bool parseActivate(String response);
  IPAddress androidIP = IPAddress();
  uint32_t androidPort = -1;

private:
};

extern MCUDP MC_UDP;
