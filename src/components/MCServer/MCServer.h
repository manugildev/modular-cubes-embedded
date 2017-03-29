#include <Arduino.h>
#include <ESP8266WebServer.h>

class MCServer {
public:
  MCServer();
  void setup();
  void loop();
  // Requests Handlers
  void handleGET();
  void handleUPDATE();
  // Request Calls
  String GET();
  bool UPDATE(String data);
  // Error Handlers
  String parseError(String data);

private:
  String homePage;
};

extern MCServer MC_Server;
