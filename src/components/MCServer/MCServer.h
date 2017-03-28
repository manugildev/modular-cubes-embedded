#include <Arduino.h>
#include <ESP8266WebServer.h>

class MCServer {
public:
  MCServer();
  void begin();
  void loop();

  // Requests Handlers
  void handleGET();
  void handlePUT();
  void handlePOST();
  void handleDELETE();

private:
  String homePage;
};

extern MCServer MC_Server;
