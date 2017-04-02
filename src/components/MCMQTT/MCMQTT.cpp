#include <ESP8266WiFi.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCUDP/MCUDP.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

#define MAXSC 6

WiFiClient mqttClient;
Adafruit_MQTT_Client mqtt(&mqttClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME,
                          AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish datafeed =
    Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/data");

Adafruit_MQTT_Subscribe dataSubscription =
    Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/data");

Adafruit_MQTT_Subscribe activateSubscription =
    Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/activate");

void MCMQTT::setup() {
  mqtt.subscribe(&activateSubscription);
  MQTT_connect();
}

void MCMQTT::loop() { parseSubscription(); }

bool MCMQTT::publish(String data) {
  if (!datafeed.publish(data.c_str())) {
    Serial.println(F("  MCMQTT -> Failed to publish data."));
    return false;
  } else {
    Serial.println(F("  MCMQTT -> Data published to MQTT server."));
    return true;
  }
}

void MCMQTT::parseSubscription() {
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20))) {
    if (subscription == &dataSubscription) {
      Serial.print(F("  MCMQTT -> Got DATA: "));
      String response = String((char *)dataSubscription.lastread);
      Serial.println((char *)dataSubscription.lastread);
      parseData(response);
    } else if (subscription == &activateSubscription) {
      Serial.print(F("  MCMQTT -> Got ACTIVATE: "));
      String response = String((char *)activateSubscription.lastread);
      Serial.println((char *)activateSubscription.lastread);
      parseActivate(response);
    }
  }
}

bool MCMQTT::parseActivate(String response) {
  DynamicJsonBuffer jsonBuffer;
  response = response.substring(1, response.length() - 1);
  JsonArray &array = jsonBuffer.parseArray(response);
  if (array.success()) {
    for (int i = 0; i < array.size(); i++) {
      String deviceId = array[i][DI_STRING];
      int activated = array[i][AC_STRING].as<int>();
      IPAddress ip;
      ip.fromString(array[i][LI_STRING].as<String>());
      if (deviceId != Cube.getDeviceId()) {
        Serial.println("  MCMQTT -> Send Activate to: " + ip.toString() +
                       " - " + array[i].as<String>());
        MC_UDP.sendPacket(ip, array[i].as<String>().c_str());
      } else {
        int activated = array[i][AC_STRING].as<int>();
        Cube.setActivated(array[i][AC_STRING] ? true : false);
      }
    }
    return true;
  } else {
    Serial.println("  MCMQTT:parseData, failed parsing Json Data.");
    return false;
  }
  return false;
}

bool MCMQTT::parseData(String response) {
  DynamicJsonBuffer jsonBuffer;
  response = response.substring(1, response.length() - 1);
  JsonObject &root = jsonBuffer.parseObject(response);
  if (root.success()) {
    root.printTo(Serial);
    return true;
  } else {
    Serial.println("  MCMQTT:parseActivate, failed parsing Json Activate.");
    return false;
  }
  return false;
}

void MCMQTT::MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected() || WiFi.status() != WL_CONNECTED) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 4;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(2000); // wait 5 seconds
    retries--;
    if (retries == 0) {
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

MCMQTT MC_MQTT;
