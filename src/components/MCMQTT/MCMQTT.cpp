#include <components/MCMQTT/MCMQTT.h>
#include <data/ModularCube.h>

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "gikdew"
#define AIO_KEY "dbd315bf60794acf9a8c51bc2e19c371"
#define MAXSC 6

WiFiClient mqttClient;
Adafruit_MQTT_Client mqtt(&mqttClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME,
                          AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish datafeed =
    Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/data");

Adafruit_MQTT_Subscribe dataSub =
    Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/data");

void MCMQTT::setup() {
  MQTT_connect();
  mqtt.subscribe(&dataSub);
}

void MCMQTT::loop() {

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) {
    if (subscription == &dataSub) {
      Serial.print(F("Got: "));
      Serial.println((char *)dataSub.lastread);
    }
  }
}

bool MCMQTT::publish(int data) {
  if (!datafeed.publish(data)) {
    Serial.println(F("  Failed to publish data."));
    return false;
  } else {
    Serial.println(F("  Data published to MQTT server."));
    return true;
  }
}

void MCMQTT::MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}
MCMQTT MC_MQTT;
