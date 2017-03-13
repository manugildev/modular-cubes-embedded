#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

// Custom
#include <Configuration.h>
#include <Firebase.h>
#include <Server.h>

void setup() {
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

  firebase.begin(configuration.firebase_host, configuration.firebase_auth);
}

void loop() {
  delay(1000);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(firebase.GET("/numbers"));
}
