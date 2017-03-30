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

private:
};

extern MCUDP MC_UDP;
