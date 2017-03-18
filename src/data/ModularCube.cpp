#include <ESP8266WiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void ModularCube::setup() {
  Serial.begin(115200);
  WiFi.begin(configuration.ssid, configuration.pass);

  // Connecting to WIFI
  Serial.print("\nConnecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }

  Serial.printf("\nConnected to %s\n", configuration.ssid);
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
}
void ModularCube::loop() {
  currentOrientation = accelerometer.getCurrentOrientation();
  Serial.println(String(currentOrientation));
  delay(1000);
}

bool ModularCube::findCubesWifi() {
  Serial.print("Trying to find Cubes_WIFI...");
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
      if (WiFi.SSID(i) == configuration.ssid) {
        return true;
      }
      delay(400);
      tries++;
      Serial.println("Cubes_WIFI not found. Trying again.");
    }
    Serial.println("Cubes_WIFI could not be found.");
    return false;
  }
}

ModularCube Cube;
