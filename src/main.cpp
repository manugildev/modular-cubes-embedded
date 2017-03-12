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

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/comments/";
  url += "1";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  String json = "";
  boolean httpBody = false;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (!httpBody && line.charAt(1) == '{') {
      httpBody = true;
    }
    if (httpBody) {
      json += line;
    }
  }

  Serial.println(json);
  Serial.println("closing connection");
}
