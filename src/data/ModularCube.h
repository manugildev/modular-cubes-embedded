#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WIFITYPE.h>

class ModularCube {
public:
  // Constructor
  ModularCube();
  // Variable Setters
  void setDeviceID(String deviceId);
  void setWlan(String wlan);
  void setLocalIP(String localIP);
  void setAPName(String APName);
  void setCurrentOrientation(int currentOrientation);
  void setConnectionMode(WiFiMode wifiMode);
  void setMaster(bool master);
  void setChilds(String childs);
  // Variable Getters
  String getDeviceId();
  String getWlan();
  String getLocalIP();
  String getAPName();
  String getChilds();
  int getCurrentOrientation();
  WiFiMode getConnectionMode();
  bool isMaster();
  // Other Methods
  void setup();
  void loop();
  String getJson();
  String getFJson();

private:
  // Variables
  unsigned int t0;
  String deviceId;
  String wlan;
  String localIP;
  String APName;
  int currentOrientation;
  WiFiMode connectionMode;
  bool master;
  String childs;
};

extern ModularCube Cube;
