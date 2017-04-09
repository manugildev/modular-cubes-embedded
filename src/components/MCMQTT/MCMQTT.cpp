#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <components/MCMQTT/MCMQTT.h>
#include <components/MCMesh/MCMesh.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void MCMQTT::setup() {
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback([&](char *topic, byte *payload, unsigned int length) {
    payload[length] = '\0';
    String response = String((char *)payload);
    if (String(topic) == MQTT_TOPIC_DATA) {
      Serial.print(F("  MCMQTT -> Got DATA: "));
      Serial.println(response);
      parseData(response);
    } else if (String(topic) == MQTT_TOPIC_ACTIVATE) {
      Serial.print(F("  MCMQTT -> Got ACTIVATE: "));
      Serial.println(response);
      parseActivate(response);
    }
  });
}

void MCMQTT::loop() {
  if (!mqttClient.connected())
    reconnect();
  mqttClient.loop();
}

void MCMQTT::reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT... ");
    if (mqttClient.connect("ESP8266 Client", MQTT_TOPIC_DATA, 0, 0, "{}")) {
      Serial.println("MQTT Connected!");
      mqttClient.subscribe(MQTT_TOPIC_ACTIVATE);
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      Serial.println("Retrying MQTT connection in 5 seconds...");
      delay(5000);
    }
  }
}

bool MCMQTT::publish(String topic, String data) {
  if (!mqttClient.publish(topic.c_str(), data.c_str())) {
    Serial.println(F("  MCMQTT -> Failed to publish data."));
    Serial.println(data);
    return false;
  } else {
    Serial.println(F("  MCMQTT -> Data published to MQTT server."));
    return true;
  }
}

bool MCMQTT::parseActivate(String response) {
  DynamicJsonBuffer jsonBuffer;
  JsonArray &array = jsonBuffer.parseArray(response);
  if (array.success()) {
    for (int i = 0; i < array.size(); i++) {
      String lIP = array[i][LI_STRING].as<String>();
      if (lIP.toInt() != Cube.getDeviceId()) {
        Serial.println("  MCMQTT -> Send Activate to: " + lIP + " - " +
                       array[i].as<String>());
        MC_Mesh.publish(lIP.toInt(), array[i].as<String>());
      } else {
        int activated = array[i][AC_STRING].as<int>();
        Cube.setActivated(array[i][AC_STRING] ? true : false);
        Serial.println("  MCMQTT -> Set Activate: " +
                       String(Cube.isActivated()));
        if (!array[i][AC_STRING])
          publish(MQTT_TOPIC_DATA, Cube.getJson().c_str());
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

MCMQTT MC_MQTT;
