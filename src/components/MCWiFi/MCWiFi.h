#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

class MCWiFi {
public:
  // Methods
  void setup();
  void loop();
  bool connectToWiFi(const char *ssid, const char *pass = "", int wait = 2000);
  bool checkIfWiFiExists(const char *ssid, int wait = 0);
  bool createWiFiAP(const char *ssid, const char *pass = "");
  // Look for WiFi networks and Generate the SSID for the current device
  String generateSSID();
  String getConnectTo(String apssid);
  // Helper Methods
  static String ipAdressToString(const IPAddress &ipAddress);

private:
  int maxDevicesPerAP;
};

extern MCWiFi MC_WiFi;
