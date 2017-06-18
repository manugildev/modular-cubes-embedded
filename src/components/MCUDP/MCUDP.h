#include <Arduino.h>
#include <WiFiUdp.h>

class MCUDP {
public:
  enum MessageType {
    Initial = 0,
    Connection = 1,
    Disconnection = 2,
    Information = 3,
    Connections = 4
  };
  MessageType msgType;

  void setup();
  void loop();
  bool startUdpServer();
  bool receivePacket();
  bool sendPacket(const IPAddress &address, int messageType, const char *msg,
                  uint16_t port = 8266);
  bool parseIncomingPacket(String data);
  bool parseAndroidPacket(IPAddress ip, uint32_t port, String incomingPacket);
  bool parseActivate(String response);
  bool parseGameMode(String gamemode);
  bool parseAllPackage(String gamemode);
  IPAddress androidIP = IPAddress();
  uint32_t androidPort = -1;

private:
};

extern MCUDP MC_UDP;
