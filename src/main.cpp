#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// Custom
#include <Server.h>
#include <Configuration.h>

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

  String url = "/numbers.json";
  String data = "";
  Serial.print("GET to https://");
  Serial.print(configuration.firebase_host);
  Serial.println(url);

  Serial.print("Result(response): ");
  if (client.connect(configuration.firebase_host, 443)) {

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
              "Host: " + configuration.firebase_host + "\r\n" +
              "User-Agent: ESP8266\r\n" +
              "Connection: close\r\n\r\n");

    delay(10);

    while(client.available()){
       char c = client.read();
       Serial.print(c);
    }
  } else {
    Serial.println("ERROR");
  }
  delay(5000);
  // HTTPClient
  HTTPClient http;
  http.begin("http://jsonplaceholder.typicode.com/users/1");
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();
  Serial.println();
  Serial.print("HTTP response code: ");
  Serial.println(httpCode);

  String payload = http.getString();
  Serial.println(payload);

  http.end();

}
