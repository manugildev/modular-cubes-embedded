#include <WiFiClientSecure.h>

class Firebase {
public:
  enum RequestType { GET_R, PUT_R, POST_R, PATCH_R, DELETE_R };
  String RequestTypeStrings[5] = {"GET", "PUT", "POST", "PATCH", "DELETE"};

  void begin(const String host, const String auth, const bool debug = false);
  String request(RequestType method, String path, String data);
  String GET(String path);
  String PUT(String path, String data);
  String POST(String path, String data);
  String PATCH(String path, String data);
  String DELETE(String path);

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
