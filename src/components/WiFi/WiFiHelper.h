#include <Arduino.h>

class WiFiHelper {
public:
  // Methods
  bool connectToWiFi(const char *ssid, const char *pass);
  bool checkIfWiFiExists(const char *ssid);
  bool createWiFiAP(const char *ssid, const char *pass);

private:
};

extern WiFiHelper WH;
