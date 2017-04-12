#include <components/MCAndroid/MCAndroid.h>
#include <ESP8266WiFi.h>

WiFiServer tcpServer(1234);
WiFiClient androidClient;

void MCAndroid::setup(){
  tcpServer.begin();
  androidClient = tcpServer.available();
}

void MCAndroid::loop(){
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


MCAndroid MC_Android;
