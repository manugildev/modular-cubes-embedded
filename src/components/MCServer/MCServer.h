#pragma once

#include <Arduino.h>
#include <ESP8266WebServer.h>

class MCServer {
public:
  MCServer();
  void setup();
  void loop();
  // Requests Handlers
  void handleGET();
  // Error Handlers
  String parseError(String data);

private:
  String homePage;
};

extern MCServer MC_Server;
