#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class MCMQTT {
public:
  void setup();
  void loop();
  void MQTT_connect();
  bool publish(String data);

  bool parseResponse(String response);
  String urlencode(String data);
  String decode(String data);
  unsigned char h2int(char c);

private:
};

extern MCMQTT MC_MQTT;
