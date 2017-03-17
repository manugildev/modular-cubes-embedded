#include <FirebaseObject.h>

JsonObject &FirebaseObject::parseJson(String json) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    return jsonBuffer.parseObject("{'error': 'parseJson() failed;, 'data':'" +
                                  json + "'}");
  }
  return root;
}
