#include <Firebase.h>
Firebase firebase;

void Firebase::begin(const String host, const String auth, const bool debug) {
  host_ = host;
  auth_ = auth;
  debug_ = debug;
}

String Firebase::GET(const String path) {
  WiFiClientSecure client;
  int status = client.connect(host_.c_str(), 443);
  if (status) {
    // Building GET Request
    client.print(String("GET "));
    client.print(path + ".json HTTP/1.1\r\n");
    client.print("Host: " + host_ + "\r\n");
    client.print("Connection: close\r\n\r\n");
    delay(10);

    // Read and Print Headers
    String headers = getHeaders(client);

    // Read ResponseCode and ResponseMessage
    String responseCode = getResponseCode(headers);
    String responseMessage = getResponseMessage(headers);

    // Read Content
    String line = client.readStringUntil('\n');

    // Build final json
    String json = "{'data': " + line + ", 'response_code': '" + responseCode +
                  "', 'response_message': '" + responseMessage + "'}";
    return json;
  } else {
    return "{'response_code': '" + String(status) + "'}";
  }
}

JsonObject &Firebase::parseJson(String json) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    return jsonBuffer.parseObject("{'error': 'parseJson() failed;'}");
  }
  return root;
}

String Firebase::getHeaders(WiFiClientSecure client) {
  String headers;
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    headers += line;
  }
  return headers;
}

String Firebase::getResponseCode(String headers) {
  String response =
      headers.substring(headers.indexOf(" ") + 1, headers.indexOf("\r"));
  return response.substring(0, response.indexOf(" "));
}

String Firebase::getResponseMessage(String headers) {
  String response =
      headers.substring(headers.indexOf(" ") + 1, headers.indexOf("\r"));
  response = response.substring(0, response.indexOf(" "));
  return response.substring(response.indexOf(" ") + 1);
}
