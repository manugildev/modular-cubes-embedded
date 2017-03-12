#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>

// Custom
#include <Server.h>
#include <Configuration.h>

const char* host = "jsonplaceholder.typicode.com";


void setup() {
  Serial.begin(9600);
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
WiFiClient client;

void loop() {
  delay(5000);

  // HTTPClient
  HTTPClient http;
  http.begin("http://jsonplaceholder.typicode.com/users/1");
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();

  Serial.println("HTTP response code: ");
  Serial.println(httpCode);

  String payload = http.getString();
  Serial.println(payload);

  http.end();
}
