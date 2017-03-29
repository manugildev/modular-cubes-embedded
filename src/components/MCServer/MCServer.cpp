#include <ArduinoJson.h>
#include <components/MCServer/MCServer.h>
#include <data/ModularCube.h>

ESP8266WebServer server(80);
MCServer::MCServer() { homePage = "Hi from the server!"; }

void MCServer::begin() {
  server.on("/get", []() { MC_Server.handleGET(); });
  server.on("/post", []() { MC_Server.handlePOST(); });
  server.on("/put", []() { MC_Server.handlePUT(); });
  server.on("/delete", []() { MC_Server.handleDELETE(); });

  server.begin();
}

void MCServer::handleGET() {
  if (Cube.isMaster()) {
    server.send(200, "text/html", Cube.getJson());
  }
}
// Update & Create
void MCServer::handlePUT() {
  if (!server.hasArg("data")) {
    Serial.println("PUT Request with no argumment('data').");
    return;
  }
  if (Cube.isMaster()) {
    String data = server.arg("data");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(Cube.getJson());
    JsonObject &element = root[Cube.getAPName()].as<JsonObject>();

    String childs = Cube.getChilds();
    JsonObject &childsObject = element["childs"].as<JsonObject>();
    JsonObject &receivedData = jsonBuffer.parseObject(data);
    String deviceName = receivedData.begin()->key;
    JsonObject &deviceData = receivedData[deviceName].as<JsonObject>();

    // This should update the value if it does exist
    childsObject[deviceName] = deviceData;
    String childString;
    childsObject.printTo(childString);
    Cube.setChilds(childString);
    server.send(200, "text/html", Cube.getJson());
  }
}
void MCServer::handlePOST() {}
void MCServer::handleDELETE() {}

void MCServer::loop() { server.handleClient(); }

MCServer MC_Server;
