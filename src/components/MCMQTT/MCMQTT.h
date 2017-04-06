#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class MCMQTT {
public:
  void setup();
  void loop();
  void reconnect();
  bool publish(String topic, String data);
  bool subscribe(String topic);

  bool parseActivate(String response);
  bool parseData(String response);
  void parseSubscription();

private:
};

extern MCMQTT MC_MQTT;
