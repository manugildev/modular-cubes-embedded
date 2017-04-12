#include <ArduinoJson.h>
#include <RestClient.h>
#include <components/MCServer/MCServer.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

ESP8266WebServer server(80);
RestClient client = RestClient("10.215.0.1");

MCServer::MCServer() { homePage = "Hi from the server!"; }

void MCServer::setup() {
  server.on("/api", HTTP_GET, []() { MC_Server.handleGET(); });
  server.begin();
}
void MCServer::loop() { server.handleClient(); }

// Get
void MCServer::handleGET() {
  server.send(HTTP_CODE_OK, "application/json", Cube.getJson());
}

String MCServer::parseError(String data) {
  return "{'error': 'parseJson() failed;, 'data':'" + data + "'}";
}

MCServer MC_Server;
