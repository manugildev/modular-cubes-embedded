#define WIFI_SSID "XXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXX"
#define CUBES_WIFI_SSID "XXXXXXXX"
#define CUBES_WIFI_PASSWORD "XXXXXXXX"
#define FIREBASE_HOST "XXXXXXXX"
#define FIREBASE_AUTH "XXXXXXXX"
#define APPLE_MIDI_SESSION "XXXXXXXX"

#define DEBUG true

class Configuration {
public:
  const char *ssid = WIFI_SSID;
  const char *pass = WIFI_PASSWORD;
  const char *cubes_ssid = CUBES_WIFI_SSID;
  const char *cubes_pass = CUBES_WIFI_PASSWORD;
  const char *firebase_host = FIREBASE_HOST;
  const char *firebase_auth = FIREBASE_AUTH;
  const char *apple_midi_session = APPLE_MIDI_SESSION;
};
extern Configuration configuration;
