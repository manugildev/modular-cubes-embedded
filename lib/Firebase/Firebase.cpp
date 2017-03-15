#include <Firebase.h>
Firebase firebase;

void Firebase::begin(const String host, const String auth, const bool debug) {
  host_ = host;
  auth_ = auth;
  debug_ = debug;
}

// Do an HTTP request to the server
String Firebase::request(const String method, const String path,
                         const String data = "") {
  WiFiClientSecure client;
  int status = client.connect(host_.c_str(), 443);
  if (status) {
    // Building GET Request
    client.print(String(method + " "));
    client.print(path + ".json HTTP/1.1\r\n");
    client.print("Host: " + host_ + "\r\n");
    switch (method) {
    case "GET":
      client.print("Connection: close\r\n\r\n");
    case "PUT":
      client.print("Content-Length: " + String(data.length()) + "\r\n\r\n");
      client.print(data);
    case "POST":
      client.print("Content-Length: " + String(data.length()) + "\r\n\r\n");
      client.print(data);
    case "PATCH":
    case "DELETE":
    }
    delay(10);

    // Read and Print Headers
    String headers = getHeaders(client);

    // Read ResponseCode and ResponseMessage
    String responseCode = getResponseCode(headers);
    String responseMessage = getResponseMessage(headers);

    // Read Content
    String responseData = getResponseData(client);

    // Build final json
    String json = "{'data': " + responseData + ", 'response_code': '" +
                  responseCode + "', 'response_message': '" + responseMessage +
                  "'}";
    return json;
  } else {
    return "{'response_code': '" + String(status) + "'}";
  }
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
    String responseData = getResponseData(client);

    // Build final json
    String json = "{'data': " + responseData + ", 'response_code': '" +
                  responseCode + "', 'response_message': '" + responseMessage +
                  "'}";
    return json;
  } else {
    return "{'response_code': '" + String(status) + "'}";
  }
}

String Firebase::PUT(const String path, const String data) {
  WiFiClientSecure client;
  int status = client.connect(host_.c_str(), 443);
  if (status) {
    // Building GET Request
    client.print(String("PUT "));
    client.print(path + ".json HTTP/1.1\r\n");
    client.print("Host: " + host_ + "\r\n");
    client.print("Content-Length: " + String(data.length()) + "\r\n\r\n");
    client.print(data);
    delay(10);

    // Read and Print Headers
    String headers = getHeaders(client);

    // Read ResponseCode and ResponseMessage
    String responseCode = getResponseCode(headers);
    String responseMessage = getResponseMessage(headers);

    // Read Content
    String responseData = getResponseData(client);

    // Build final json
    String json = "{'data': " + responseData + ", 'response_code': '" +
                  responseCode + "', 'response_message': '" + responseMessage +
                  "'}";
    return json;
  } else {
    return "{'response_code': '" + String(status) + "'}";
  }
}

String Firebase::POST(const String path, const String data) {
  WiFiClientSecure client;
  int status = client.connect(host_.c_str(), 443);
  if (status) {
    // Building GET Request
    client.print(String("POST "));
    client.print(path + ".json HTTP/1.1\r\n");
    client.print("Host: " + host_ + "\r\n");
    client.print("Content-Length: " + String(data.length()) + "\r\n\r\n");
    client.print(data);
    delay(10);

    // Read and Print Headers
    String headers = getHeaders(client);

    // Read ResponseCode and ResponseMessage
    String responseCode = getResponseCode(headers);
    String responseMessage = getResponseMessage(headers);

    // Read Content
    String responseData = getResponseData(client);

    // Build final json
    String json = "{'data': " + responseData + ", 'response_code': '" +
                  responseCode + "', 'response_message': '" + responseMessage +
                  "'}";
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
  String h = headers;
  String response = h.substring(h.indexOf(" ") + 1, h.indexOf("\r"));
  return response.substring(0, response.indexOf(" "));
}

String Firebase::getResponseMessage(String headers) {
  String h = headers;
  String response = h.substring(h.indexOf(" ") + 1, h.indexOf("\r"));
  return response.substring(response.indexOf(" ") + 1);
}

String Firebase::getResponseData(WiFiClientSecure client) {
  String rD = "";
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line == NULL) {
      break;
    }
    rD += line;
  }
  return rD;
}
