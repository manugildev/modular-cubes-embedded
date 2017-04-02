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

Adafruit_MQTT_Subscribe dataSubscription =
    Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/data");

void MCMQTT::setup() {
  // mqtt.subscribe(&dataSubscription);
  MQTT_connect();
}

void MCMQTT::loop() {
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(50))) {
    if (subscription == &dataSubscription) {
      Serial.print(F("  MCMQTT->Got: "));
      Serial.println((char *)dataSubscription.lastread);
    }
  }
}

bool MCMQTT::publish(String data) {
  if (!datafeed.publish(data.c_str())) {
    Serial.println(F("  Failed to publish data."));
    return false;
  } else {
    Serial.println(F("  Data published to MQTT server."));
    return true;
  }
}

bool MCMQTT::parseResponse(String response) {
  DynamicJsonBuffer jsonBuffer;
  response = response.substring(1, response.length() - 2);
  JsonObject &root = jsonBuffer.parseObject(response);
  if (root.success()) {
    root.printTo(Serial);
    return true;
  } else {
    Serial.println("Fucking error you motherfucker");
    return false;
  }
  return false;
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

String MCMQTT::urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      // encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}

unsigned char MCMQTT::h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

MCMQTT MC_MQTT;
