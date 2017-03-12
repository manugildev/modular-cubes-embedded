#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <UniversalTelegramBot.h>

// Custom
#include <Server.h>
#include <Configuration.h>

#define BotToken "XXXXXXXXXXXXXXXXXXXXX"
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BotToken, secured_client);

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
  delay(5000);

  bot.sendMessage("chat_id", "Test Message from ESP8266", "Markdown");

  // HTTPClient
  HTTPClient http;
  http.begin("http://jsonplaceholder.typicode.com/users/1");
  http.addHeader("Content-Type", "text/plain");
  int httpCode = http.GET();

  Serial.print("HTTP response code: ");
  Serial.println(httpCode);

  String payload = http.getString();
  Serial.println(payload);

  http.end();

}
