#include <Arduino.h>
#include <ESP8266WiFi.h>

class MCAndroid {
public:
  void setup();
  void loop();
  String getResponse();
  void sendConnection(String data);
  void sendDisconnection(String data);
  void sendInformation(String data);
  IPAddress androidIP = IPAddress();
};

extern MCAndroid MC_Android;
