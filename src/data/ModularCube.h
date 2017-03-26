#include <Arduino.h>
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
  // Variable Getters
  String getDeviceName();
  String getDeviceId();
  String getWlan();
  String getLocalIP();
  int getCurrentOrientation();
  WiFiMode getConnectionMode();
  // Other Methods
  String getJson();
  void setup();
  void loop();

private:
  // Variables
  unsigned int t0;
  String deviceName;
  String deviceId;
  String wlan;
  String localIP;
  int currentOrientation;
  WiFiMode connectionMode;

  // Midi midi;
  // Accelerometer accelerometer;
};

extern ModularCube Cube;
