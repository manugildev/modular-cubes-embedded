#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WIFITYPE.h>

class ModularCube {
public:
  // Constructor
  ModularCube();
  // Variable Setters
  void setDeviceName(String dN);
  void setDeviceID(String deviceId);
  void setWlan(String wlan);
  void setLocalIP(String localIP);
  void setCurrentOrientation(int currentOrientation);
  void setConnectionMode(WiFiMode wifiMode);
  void setMaster(bool master);
  // Variable Getters
  String getDeviceName();
  String getDeviceId();
  String getWlan();
  String getLocalIP();
  int getCurrentOrientation();
  WiFiMode getConnectionMode();
  bool isMaster();
  // Other Methods
  void setup();
  void loop();
  void setUpWiFi();
  String getJson();

private:
  // Variables
  unsigned int t0;
  String deviceName;
  String deviceId;
  String wlan;
  String localIP;
  int currentOrientation;
  WiFiMode connectionMode;
  bool master;
};

extern ModularCube Cube;
