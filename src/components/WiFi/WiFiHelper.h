#include <Arduino.h>

class WiFiHelper {
public:
  // Methods
  bool connectToWiFi(const char *ssid, const char *pass);
  bool checkIfWiFiExists(const char *ssid);
  bool createWiFiAP(const char *ssid, const char *pass);

  // Look for WiFi networks and Generate the SSID for the current device
  String generateSSID();

private:
};

extern WiFiHelper WH;
