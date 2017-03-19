#include <ESP8266WiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void ModularCube::setup() {
  connectToWifi(configuration.ssid, configuration.pass);
}
void ModularCube::loop() {
  currentOrientation = accelerometer.getCurrentOrientation();
  Serial.println(String(currentOrientation));
}

// Connect to any wify
bool ModularCube::connectToWifi(const char *ssid, const char *pass) {
  WiFi.begin(ssid, pass);
  Serial.printf("\nConnecting to %d", ssid);
  int tries = 0;
  // We try to connect for ten times, if it doesn't work we just stop
  while (tries < 10) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(400);
      tries++;
    } else {
      Serial.printf("\nConnected to %s.\n", ssid);
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
      return true;
    }
    Serial.printf("\nConnection to %s failed.", ssid);
    return false;
  }
}

bool ModularCube::checkIfWifiExists(const char *ssid) {
  Serial.printf("Trying to find %s...", ssid);
  int tries = 0;

  // We set the WiFi to Station Mode and Disconnect from any other WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (tries < 5) {
    int n = WiFi.scanNetworks();
    Serial.print(n);
    Serial.println(" network(s) found");
    for (int i = 0; i < n; i++) {
      Serial.println(WiFi.SSID(i));
      if (WiFi.SSID(i) == ssid) {
        Serial.printf("%s found.", ssid);
        return true;
      }
      delay(400);
      tries++;
      Serial.printf("%s not found. Trying again.\n", ssid);
    }
    Serial.printf("%s could not be found.\n", ssid);
    return false;
  }
}

ModularCube Cube;
