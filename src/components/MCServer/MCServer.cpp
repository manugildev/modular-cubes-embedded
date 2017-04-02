#include <ArduinoJson.h>
#include <RestClient.h>
#include <components/MCServer/MCServer.h>
#include <data/ModularCube.h>

ESP8266WebServer server(80);
RestClient client = RestClient("192.168.4.1");

MCServer::MCServer() { homePage = "Hi from the server!"; }

void MCServer::setup() {
  server.on("/api", HTTP_GET, []() { MC_Server.handleGET(); });
  server.on("/api", HTTP_PUT, []() { MC_Server.handleUPDATE(); });
  server.begin();
}
void MCServer::loop() { server.handleClient(); }

// Get
void MCServer::handleGET() {
  if (Cube.isMaster()) {
    server.send(HTTP_CODE_OK, "application/json", Cube.getFJson());
  }
}
// Update & Create
void MCServer::handleUPDATE() {
  if (!server.hasArg("data")) {
    Serial.println("PUT Request with no argumment('data').");
    server.send(HTTP_CODE_BAD_REQUEST, "application/json", Cube.getJson());
  } else {
    // TODO: If the cube is Master we update the childs, if its not master we
    // update the childs and do an update request to the upper level
    String data = server.arg("data");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(Cube.getJson());
    JsonObject &element = root[Cube.getDeviceId()].as<JsonObject>();

    String childs = Cube.getChilds();
    JsonObject &childsObject = element["childs"].as<JsonObject>();
    JsonObject &receivedData = jsonBuffer.parseObject(data);

    if (!receivedData.success()) {
      server.send(HTTP_CODE_BAD_REQUEST, "application/json", parseError(data));
      return;
    }
    // Update if the value does not exist
    String deviceName = receivedData.begin()->key;
    JsonObject &deviceData = receivedData[deviceName].as<JsonObject>();
    childsObject[deviceName] = deviceData;
    String childString;
    childsObject.printTo(childString);
    Cube.setChilds(childString);
    server.send(HTTP_CODE_OK, "application/json", Cube.getFJson());

    // If the current cube is not the master, upload to the upper node
  }
}

String MCServer::GET() {
  if (WiFi.status() == WL_CONNECTED) {
    String response;
    int statusCode = client.get("/api", &response);
    if (statusCode == 200) {
      return response;
    } else {
      Serial.println("MCServer::GET():" + Cube.getAPName() +
                     " request error (" + statusCode + ")");
      return "";
    }
  } else {
    Serial.println("MCServer::GET():" + Cube.getAPName() + " not connected.");
    return "";
  }
}

bool MCServer::UPDATE(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    String response;
    String url = "/api?data=" + data;
    int statusCode = client.put(url.c_str(), "", &response);
    if (statusCode == 200) {
      return true;
    } else {
      Serial.println("MCServer::UPDATE():" + Cube.getAPName() +
                     " request error (" + statusCode + ")");
      return false;
    }
  } else {
    Serial.println("MCServer::UPDATE():" + Cube.getAPName() +
                   " not connected.");
    return false;
  }
}

String MCServer::parseError(String data) {
  return "{'error': 'parseJson() failed;, 'data':'" + data + "'}";
}

MCServer MC_Server;
