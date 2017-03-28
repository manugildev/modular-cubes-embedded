#include <Arduino.h>

class WiFiHelper {
public:
  // Methods
  bool connectToWiFi(const char *ssid, const char *pass = "", int times = 40);
  bool checkIfWiFiExists(const char *ssid, int times = 1);
  bool createWiFiAP(const char *ssid, const char *pass = "");

  // Look for WiFi networks and Generate the SSID for the current device
  String generateSSID();
  String getConnectTo(String apssid);

private:
};

extern WiFiHelper WH;
