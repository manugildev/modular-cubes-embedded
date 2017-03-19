#include <AppleMidi.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <components/Midi/MidiHelper.h>
#include <configuration/Configuration.h>

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);
unsigned int t0 = millis();

void MidiHelper::setup() {
  isConnected = false;
  Serial.println("\nCreating AppleMidi Session.");
  Serial.printf(" - Session Name: %s\n", configuration.apple_midi_session);
  Serial.printf(" - Host/Port: %s:5004\n", String(WiFi.localIP()).c_str());

  AppleMIDI.begin(configuration.apple_midi_session);
}

void MidiHelper::loop() {
  // Listen to incoming notes
  AppleMIDI.run();

  // send a note every second
  // (dont cáll delay(1000) as it will stall the pipeline)
  if ((millis() - t0) > 1000) {
    t0 = millis();
    //   Serial.print(".");

    int note = 45;
    int velocity = 55;
    int channel = 1;

    AppleMIDI.noteOn(note, velocity, channel);
    AppleMIDI.noteOff(note, velocity, channel);
  }
}

MidiHelper Midi;
