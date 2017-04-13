#include <ArduinoJson.h>
#include <components/GameLogic/GameLogic.h>
#include <components/MCMesh/MCMesh.h>
#include <data/ModularCube.h>

#define BUILT_IN_LED 2
#define BUILT_IN_BUTTON 16
#define START_MS_INTERVAL 3000
#define MS_MINIMUM 200
#define MS_DECREMENT -100
#define MS_INCREMENT +100

void GameLogic::setup() {
  pinMode(BUILT_IN_LED, OUTPUT);
  pinMode(BUILT_IN_BUTTON, INPUT);
}

void GameLogic::loop() {
  if (Cube.isActivated() && millis() > endTime) {
    digitalWrite(BUILT_IN_LED, HIGH);
    switchRandomLightInMesh(MS_DECREMENT);
  } else if (Cube.isActivated() && millis() < endTime) {
    digitalWrite(BUILT_IN_LED, LOW);
  } else if (!Cube.isActivated()){
    digitalWrite(BUILT_IN_LED, HIGH);
    if(MC_Mesh.mesh.getNodeList().size()==0) switchOnLight(START_MS_INTERVAL);
  }
}

void GameLogic::switchOnLight(int milliseconds) {
  currentMilliseconds = milliseconds;
  Serial.println("  GL -> switchOnLight(" + String(milliseconds) + ")");
  startTime = millis();
  endTime = millis() + milliseconds;
  Cube.setActivated(true);
  Cube.refreshData();
  // TODO: Move this to the SetActivate Function?
}

void GameLogic::switchRandomLightInMesh(int addSeconds) {
  Cube.setActivated(false);
  Cube.refreshData();

  uint32_t randomNode = MC_Mesh.getRandomNode();
  Serial.println("  GL -> switchRandomLightInMesh(" + String(randomNode) + ")");
  if (randomNode == Cube.getDeviceId() || randomNode == 0) {
    switchOnLight(max(MS_MINIMUM, currentMilliseconds));
    return;
  }
  if (randomNode != 0)
    if (!MC_Mesh.publish(randomNode, createJsonLight(randomNode, addSeconds))) {
      switchOnLight(max(MS_MINIMUM, currentMilliseconds));
    }
}

String GameLogic::createJsonLight(uint32_t randomNode, int addSeconds) {
  DynamicJsonBuffer jsonBuffer;
  JsonArray &array = jsonBuffer.createArray();
  JsonObject &data = jsonBuffer.createObject();
  data["lIP"] = randomNode;
  data["light"] = 1;
  data["time"] = max(MS_MINIMUM, currentMilliseconds+addSeconds);
  array.add(data);
  String msg;
  data.printTo(msg);
  return msg;
}

void GameLogic::clicked(){
  Serial.println("  GL -> CLICKED!");
  switchRandomLightInMesh(MS_INCREMENT);
}

GameLogic GL;
