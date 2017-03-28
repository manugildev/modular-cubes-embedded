#include <ArduinoJson.h>
#include <components/MCServer/MCServer.h>
#include <data/ModularCube.h>

ESP8266WebServer server(80);
MCServer::MCServer() { homePage = "Hi from the server!"; }

void MCServer::begin() {
  server.on("/", HTTP_GET, []() {
    Serial.println("ARGS:" + server.arg("data"));
    MC_Server.handleGET();
  });
  server.on("/", HTTP_POST, []() { MC_Server.handlePOST(); });
  server.on("/", HTTP_PUT, []() {
    Serial.println("ARGS:" + server.args());
    MC_Server.handlePUT();
  });
  server.on("/", HTTP_DELETE, []() { MC_Server.handleDELETE(); });

  server.begin();
}

void MCServer::handleGET() {
  if (Cube.isMaster()) {
    server.send(200, "text/html", Cube.getJson());
  }
}
void MCServer::handlePUT() {
  String message = "Number of args received: ";
  message += server.args();
  for (int i = 0; i < server.args(); i++) {
    message += "Arg nº" + (String)i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
  }
  Serial.println(message);

  if (Cube.isMaster()) {
    String data = server.arg("data"); // TODO: Make this a must
    String childs = Cube.getChilds();
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(Cube.getJson());
    JsonObject &element = root[Cube.getAPName()].as<JsonObject>();
    element.printTo(Serial);
    Serial.println();
    JsonArray &childArray = element.createNestedArray("childs");
    childArray.add(data); // TODO: check this
    root.printTo(Serial);
    Serial.println();

    server.send(200, "text/html", Cube.getJson());
  }
}
void MCServer::handlePOST() {}
void MCServer::handleDELETE() {}

void MCServer::loop() { server.handleClient(); }

MCServer MC_Server;
