#include <ArduinoJson.h>
#include <components/GameLogic/GameLogic.h>
#include <components/MCMesh/MCMesh.h>
#include <data/ModularCube.h>

void GameLogic::setup() {}

void GameLogic::loop() {
  if (Cube.isActivated() && millis() > endTime) {
    digitalWrite(2, HIGH);
    Cube.setActivated(false);
    switchRandomLightInMesh(-100);
  } else if (millis() < endTime) {
    digitalWrite(2, LOW);
  } else {
    digitalWrite(2, HIGH);
    if(MC_Mesh.mesh.getNodeList().size()==0){
      switchOnLight(3000);
    }
  }
}

void GameLogic::switchOnLight(int milliseconds) {
  currentMilliseconds = milliseconds;
  Serial.println("  GL -> switchOnLight(" + String(milliseconds) + ")");
  startTime = millis();
  endTime = millis() + milliseconds;
  Cube.setActivated(true);
  // TODO: Move this to the SetActivate Function?
}

void GameLogic::switchRandomLightInMesh(int addSeconds) {
  // This is just to be sure that the light turns off, and that we change.
  //digitalWrite(2, HIGH);
  //Cube.setActivated(false);

  uint32_t randomNode = MC_Mesh.getRandomNode();
  Serial.println("  GL -> switchRandomLightInMesh(" + String(randomNode) + ")");
  if (randomNode == Cube.getDeviceId() || randomNode == 0) {
    switchOnLight(max(200, currentMilliseconds + addSeconds));
    return;
  }
  if (randomNode != 0)
    if (!MC_Mesh.publish(randomNode, createJsonLight(randomNode, addSeconds))) {
      switchOnLight(max(200, currentMilliseconds + addSeconds));
    }
}

String GameLogic::createJsonLight(uint32_t randomNode, int addSeconds) {
  DynamicJsonBuffer jsonBuffer;
  JsonArray &array = jsonBuffer.createArray();
  JsonObject &data = jsonBuffer.createObject();
  data["lIP"] = randomNode;
  data["light"] = 1;
  data["time"] = max(200, currentMilliseconds+addSeconds);
  array.add(data);
  String msg;
  data.printTo(msg);
  return msg;
}

GameLogic GL;
