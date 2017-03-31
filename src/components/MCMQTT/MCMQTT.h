#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ESP8266WiFi.h>

class MCMQTT {
public:
  void setup();
  void loop();
  void MQTT_connect();
  bool publish(int data);

private:
};

extern MCMQTT MC_MQTT;
