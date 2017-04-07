#include <Arduino.h>
#include <easyMesh.h>

class MCEasyMesh {
public:
  int t0;
  void setup();
  void loop();
  void setUpCallbacks();
  void publish(uint32_t destId, String msg);
  void publishToAll(String msg);

  void setMasterIfMeshDoesNotExists();
  bool checkIfMeshCreated(const char *prefix, int wait = 0);
};

extern MCEasyMesh MC_EasyMesh;
