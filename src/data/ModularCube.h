#include <Accelerometer.h>
#include <Arduino.h>

class ModularCube {
public:
  // Variable Setters
  void setDeviceName(String deviceName);
  void setDeviceID(String deviceId);
  void setWlan(String wlan);
  void setLocalIP(String localIP);
  void setCurrentOrientation(int currentOrientation);

  // Variable Getters
  String getDeviceName();
  String getDeviceId();
  String getWlan();
  String getLocalIP();
  int getCurrentOrientation();

  // Other Methods
  String getJson();
  void setup();
  void loop();

  // TODO: Change this methods to a wifi specific class
  bool findCubesWifi();

private:
  String deviceName;
  String deviceId;
  String wlan;
  String localIP;
  int currentOrientation;

  // connectionmode
  // Midi midi;
  // Accelerometer accelerometer;
};

extern ModularCube Cube;
