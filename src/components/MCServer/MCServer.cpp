#include <components/MCServer/MCServer.h>

ESP8266WebServer server(80);
MCServer::MCServer() { homePage = "Hi from the server!"; }

void MCServer::begin() {
  server.on("/", HTTP_GET, []() { MC_Server.handleGET(); });
  server.on("/", HTTP_POST, []() { MC_Server.handlePOST(); });
  server.on("/", HTTP_PUT, []() { MC_Server.handlePUT(); });
  server.on("/", HTTP_DELETE, []() { MC_Server.handleDELETE(); });

  server.begin();
}

void MCServer::handleGET() { server.send(200, "text/html", homePage); }
void MCServer::handlePUT() {}
void MCServer::handlePOST() {}
void MCServer::handleDELETE() {}

void MCServer::loop() { server.handleClient(); }

MCServer MC_Server;
