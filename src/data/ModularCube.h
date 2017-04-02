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
  void setChilds(String childs);
  void setConnectionMode(WiFiMode wifiMode);
  void setCurrentOrientation(int currentOrientation);
  void setMaster(bool master);
  void setActivated(bool activated);
  // Variable Getters
  String getDeviceId();
  String getWlan();
  String getLocalIP();
  String getAPName();
  String getChilds();
  WiFiMode getConnectionMode();
  int getCurrentOrientation();
  bool isMaster();
  bool isActivated();
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
  String childs;
  WiFiMode connectionMode;
  int currentOrientation;
  bool activated;
  bool master;
};

extern ModularCube Cube;
