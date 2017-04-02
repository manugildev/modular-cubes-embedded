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

  bool parseActivate(String response);
  bool parseData(String response);
  void parseSubscription();

private:
};

extern MCMQTT MC_MQTT;
