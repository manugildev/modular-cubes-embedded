#include <components/WiFi/WiFiHelper.h>

// TODO: Variable parameters for Seconds you want to try
// Connect to any wifi
bool WiFiHelper::connectToWiFi(const char *ssid, const char *pass) {
  WiFi.begin(ssid, pass);
  Serial.printf("\nConnecting to %s", ssid);
  int tries = 0;
  // We try to connect for ten times, if it doesn't work we just stop
  while (tries < 20) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(800);
      tries++;
    } else if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("\nConnected to %s.\n", ssid);
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
      return true;
    } else if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.printf("\nConnection to %s failed.\n", ssid);
      return false;
    }
  }
  Serial.printf("\nConnection to %s failed.\n", ssid);
  return false;
}

// TODO: Variable parameters for Seconds you want to try
// Checks if the wifi passed in the parameters exist, returns true or false
bool WiFiHelper::checkIfWiFiExists(const char *ssid) {
  Serial.printf("\nTrying to find %s...\n", ssid);
  int tries = 0;

  // We set the WiFi to Station Mode and Disconnect from any other WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (tries < 5) {
    int n = WiFi.scanNetworks();
    Serial.printf("%i - %i network(s) found. ", tries, n);
    for (int i = 0; i < n; i++) {
      if (WiFi.SSID(i) == ssid) {
        return true;
      }
    }
    Serial.printf("%s Not found.\n", ssid);
    delay(400);
    tries++;
  }
  Serial.printf("After %i tries %s could not be found.\n", tries, ssid);
  return false;
}

// Creates WiFi Hotspot
bool WiFiHelper::createWiFiAP(const char *ssid, const char *pass) {
  Serial.println();
  Serial.printf("Starting Access Point: %s\n", ssid);
  WiFi.softAP(ssid, pass);
  String ip = WiFi.softAPIP().toString();
  Serial.printf("AP IP adress: %s", ip.c_str());
}

WiFiHelper WH;
