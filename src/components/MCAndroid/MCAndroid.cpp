#include <components/MCAndroid/MCAndroid.h>

WiFiServer tcpServer(1234);
WiFiClient androidClient;

void MCAndroid::setup() {
  tcpServer.begin();
  androidClient = tcpServer.available();
}

void MCAndroid::loop() {
  if (!androidClient.connected()) {
    androidClient = tcpServer.available();
  } else {
    if (androidClient.available() > 0) {
      String req = androidClient.readStringUntil('\n');
      Serial.println(req);
    }
  }
}

String MCAndroid::getResponse() {
  String response;
  while (androidClient.connected()) {
    String line = androidClient.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    response += line;
  }
  return response;
}

void MCAndroid::sendConnection(String data) {}

void MCAndroid::sendDisconnection(String data) {}

void MCAndroid::sendInformation(String data) {}

MCAndroid MC_Android;
