#define WIFI_SSID "XXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXX"
#define CUBES_WIFI_SSID "XXXXXXXXX"
#define CUBES_WIFI_PASSWORD "XXXXXXXXX"
#define FIREBASE_HOST "XXXXXXXXX"
#define FIREBASE_AUTH "XXXXXXXXX"

#define DEBUG true

class Configuration {
public:
  const char *ssid = WIFI_SSID;
  const char *pass = WIFI_PASSWORD;
  const char *cubes_ssid = CUBES_WIFI_SSID;
  const char *cubes_pass = CUBES_WIFI_PASSWORD;
  const char *firebase_host = FIREBASE_HOST;
  const char *firebase_auth = FIREBASE_AUTH;
};
extern Configuration configuration;
