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
void ModularCube::loop() {}

ModularCube Cube;
