#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

class Firebase {
public:
  void begin(const String host, const String auth, const bool debug = false);
  String GET(const String path);
  String PUT(String path, String data);
  String POST(String path, String data);
  String PATCH(String path, String data);
  String DELETE(String path);

  JsonObject &parseJson(String json);
  String getHeaders(WiFiClientSecure client);
  String getResponseCode(String headers);
  String getResponseMessage(String headers);
  String getResponseData(WiFiClientSecure client);

private:
  bool debug_;
  String host_;
  String auth_;
};

extern Firebase firebase;
