#include <ESP8266WiFi.h>
#include <components/MCWiFi/MCWiFi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

void MCWiFi::setup() {
  pinMode(2, OUTPUT); // Just to blink whenever is connecting
  maxDevicesPerAP = 4;
  String mssid = String(String(configuration.cubes_ssid) + "_M");
  String wifiName = generateSSID();
  String connectTo = getConnectTo(wifiName);
  createWiFiAP(wifiName.c_str());
  if (wifiName == mssid) {
    Cube.setMaster(true);
    if (connectToWiFi(connectTo.c_str(), configuration.pass, 10000))
      Cube.setWlan(connectTo);
    WiFi.mode(WIFI_AP_STA);
  } else {
    if (connectToWiFi(connectTo.c_str(), "", 10000))
      Cube.setWlan(connectTo);
    WiFi.mode(WIFI_STA); // TODO: WTF (Connection mode makes the whole fail)
  }
  Cube.setAPName(wifiName);
  Cube.setLocalIP(ipAdressToString(WiFi.localIP()));
  Cube.setConnectionMode(WiFi.getMode());
}

void MCWiFi::loop() {
  if (!WiFi.isConnected()) {
    Serial.println("Disconnected: Rebooting...");
    ESP.restart();
  }
}

// Connect to any wifi
bool MCWiFi::connectToWiFi(const char *ssid, const char *pass, int wait) {
  WiFi.begin(ssid, pass);
  Serial.printf("\nConnecting to %s ", ssid);
  int tries = 0;
  int maxTries = (wait / 100) + 1;
  // We try to connect for X times, if it doesn't work we just stop
  while (tries < maxTries) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      digitalWrite(2, LOW);
      delay(50);
      digitalWrite(2, HIGH);
      delay(50);
      tries++;
    } else if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("\nConnected to %s\n", ssid);
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
      return true;
    } else if (WiFi.status() == WL_CONNECT_FAILED) {
      digitalWrite(2, LOW);
      Serial.printf("\nConnection to %s failed.\n", ssid);
      return false;
    }
  }

  digitalWrite(2, LOW);
  Serial.printf("\nConnection to %s failed.\n", ssid);
  return false;
}

// Checks if the wifi passed in the parameters exist, returns true or false
bool MCWiFi::checkIfWiFiExists(const char *ssid, int wait) {
  Serial.printf("Trying to find %s...\n", ssid);
  int tries = 0;
  int maxTries = (wait / 10) + 1;
  // We set the WiFi to Station Mode and Disconnect from any other WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  while (tries < maxTries) {
    int n = WiFi.scanNetworks();
    Serial.printf("%i - %i network(s) found. ", tries, n);
    for (int i = 0; i < n; i++) {
      if (WiFi.SSID(i) == ssid) {
        return true;
      }
    }
    Serial.printf("%s Not found.\n", ssid);
    delay(10);
    tries++;
  }
  Serial.printf("After %i tries %s could not be found.\n", tries, ssid);
  return false;
}

// Creates WiFi Hotspot
bool MCWiFi::createWiFiAP(const char *ssid, const char *pass) {
  Serial.printf("Starting Access Point: %s\n", ssid);
  WiFi.softAP(ssid, pass);
  String ip = WiFi.softAPIP().toString();
  Serial.printf("AP IP adress: %s", ip.c_str());
}

// Algorithm that generates an ssid name for the current cube based on the cubes
// that are already around
String MCWiFi::generateSSID() {
  const char *mssid = String(String(configuration.cubes_ssid) + "_M").c_str();
  String smssid = String(String(configuration.cubes_ssid) + "_M");

  if (!checkIfWiFiExists(mssid)) {
    Cube.setMaster(true);
    return mssid;
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    int n = WiFi.scanNetworks();
    int maxNumber = 0;
    Serial.printf("%i network(s) found. \n", n);
    // This bucle should return the maximum index of the network
    for (int i = 0; i < n; i++) {
      String curr_ssid = WiFi.SSID(i);
      if (curr_ssid.startsWith(configuration.cubes_ssid) &&
          curr_ssid != smssid) {
        int number = curr_ssid.substring(curr_ssid.indexOf("_") + 1).toInt();
        Serial.printf("Number found %i\n", number);
        if (number > maxNumber) {
          maxNumber = number;
        }
      }
    }

    String previous = "";
    if (maxNumber == 0) {
      return String(String(configuration.cubes_ssid) + "_0001");
    } else {
      char buffer[4] = "";
      sprintf(buffer, "%04d", maxNumber);
      previous = buffer;
    }
    // Generate the SSID based on the number
    String nT = previous;
    char nA[nT.length() + 1];
    strcpy(nA, nT.c_str());
    // Reverse to work with it
    const size_t len = strlen(nA);
    for (size_t i = 0; i < len / 2; i++)
      std::swap(nA[i], nA[len - i - 1]);

    // TODO: Make this so it can fill the gaps
    for (int i = 0; i < strlen(nA); i++) {
      int tN = nA[i] - '0'; // Convert char to int
      if (tN >= maxDevicesPerAP) {
        nA[i] = '1';
      } else {
        nA[i] = (tN + 1) + '0';
        break;
      }
    }

    previous = String(nA);
    // Reverse the string to return final string
    for (size_t i = 0; i < len / 2; i++)
      std::swap(nA[i], nA[len - i - 1]);

    String next = String(nA);
    return String(String(configuration.cubes_ssid) + "_" + next);
  }
}

// Gets the name of the network this cube should be connecting to, based on the
// name that the generateSSID algorithm generated
String MCWiFi::getConnectTo(String apssid) {
  String mssid = String(String(configuration.cubes_ssid) + "_M");
  if (apssid == mssid) {
    return configuration.ssid;
  } else {
    int start = apssid.indexOf("_") + 1;
    int end = apssid.length();
    int number = apssid.substring(start, end).toInt();

    // If the nodes are primary
    if (number > 0 && number <= maxDevicesPerAP)
      return mssid;
    // If the nodes are not primary
    char buffer[4] = "";
    String snum = String(number);
    int finalNum = snum.substring(0, snum.length() - 1).toInt();
    sprintf(buffer, "%04d", finalNum);
    return String(String(configuration.cubes_ssid) + "_" + buffer);
  }
}

String MCWiFi::ipAdressToString(const IPAddress &ipAddress) {
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) +
         String(".") + String(ipAddress[2]) + String(".") +
         String(ipAddress[3]);
}

MCWiFi MC_WiFi;
