#include <Arduino.h>
#include <painlessMesh.h>

class MCMesh {
public:
  int t0;
  painlessMesh mesh;

  void setup();
  void loop();
  void setUpCallbacks();
  bool publish(uint32_t destId, String msg);
  bool publishToAll(String msg);

  bool checkIfMeshExists(const char *ssid, int wait = 0);
  void setMasterIfMeshDoesNotExist();

  // GameLogic Methods
  uint32_t getRandomNode();

  // Parse Methods
  bool parseJsonChilds(String data);
  bool parseIncomingPacket(uint32_t from, String data);
  bool parseGameLight(String data);
  bool parseActivate(uint32_t master, String data);
};

extern MCMesh MC_Mesh;
