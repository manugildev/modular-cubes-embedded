#include <string>

#define FIREBASE_HOST "XXXXXXXXX"
#define FIREBASE_AUTH "XXXXXXXXXX"
#define WIFI_SSID "XXXXXXX"
#define WIFI_PASSWORD "XXXXXXX"
#define DEBUG true

class Configuration {
  public:
    const char *ssid = WIFI_SSID;
    const char *pass = WIFI_PASSWORD;
    const char *firebase_host = FIREBASE_HOST;
    const char *firebase_auth = FIREBASE_AUTH;

};

extern Configuration configuration;
