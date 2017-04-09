#include <Arduino.h>
#include <painlessMesh.h>

class MCMesh {
public:
  int t0;
  void setup();
  void loop();
  void setUpCallbacks();
  void publish(uint32_t destId, String msg);
  void publishToAll(String msg);

  bool checkIfMeshExists(const char *ssid, int wait = 0);
  void setMasterIfMeshDoesNotExist();

  bool parseJsonChilds(String data);
  bool parseIncomingPacket(uint32_t from, String data);
};

extern MCMesh MC_Mesh;
