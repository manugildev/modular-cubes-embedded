#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>

// Custom
#include <Configuration.h>
#include <Server.h>

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

void loop() {
  WiFiClientSecure client;

  String url = "/" + WiFi.macAddress() + ".json";
  String data = String(millis() / 1000);

  Serial.print("GET to https://");
  Serial.print(configuration.firebase_host);
  Serial.println(url);

  Serial.print("Result(response): ");

  if (client.connect(configuration.firebase_host, 443)) {
    client.print(String("POST ") + url + " HTTP/1.1\r\n" + "Host: " +
                 configuration.firebase_host + "\r\n" + "Content-Length: " +
                 data.length() + "\n\n" + data);

    delay(10);
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  } else {
    Serial.println("ERROR");
  }
}
