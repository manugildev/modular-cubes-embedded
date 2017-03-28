#include <AppleMidi.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <components/MCMidi/MCMidi.h>
#include <configuration/Configuration.h>
#include <data/ModularCube.h>

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI);

MCMidi::MCMidi() {
  isConnected = false;
  t0 = millis();
}

void MCMidi::setup() {
  Serial.println("\nCreating AppleMidi Session.");
  Serial.printf(" - Session Name: %s\n", configuration.apple_midi_session);
  IPAddress ip = WiFi.localIP();
  Serial.print(" - Host/Port: ");
  Serial.print(ip);
  Serial.println(":5004");

  AppleMIDI.begin(configuration.apple_midi_session);
  setupCallbacks();
}

void MCMidi::loop() {
  // Listen to incoming notes
  AppleMIDI.run();

  // TODO: Change the time variable to run every X seconds
  if (isConnected && (millis() - t0) > 1000) {
    t0 = millis();
    int note = Cube.getCurrentOrientation();
    int velocity = 55;
    int channel = 1;

    AppleMIDI.noteOn(note, velocity, channel);
    AppleMIDI.noteOff(note, velocity, channel);
  }
}

void MCMidi::setupCallbacks() {
  AppleMIDI.OnConnected([](uint32_t ssrc, char *name) {
    MC_Midi.isConnected = true;
    Serial.printf(" - Connected to Session: \n", name);
  });

  AppleMIDI.OnDisconnected([](uint32_t ssrc) {
    MC_Midi.isConnected = false;
    Serial.printf(" - Disconnected\n");
  });

  AppleMIDI.OnReceiveNoteOn([](byte channel, byte note, byte velocity) {
    Serial.print(" - Incoming NoteOn from channel: ");
    Serial.print(channel);
    Serial.print(" - Note: ");
    Serial.print(note);
    Serial.print(" - Velocity: ");
    Serial.print(velocity);
    Serial.println();
  });

  AppleMIDI.OnReceiveNoteOff([](byte channel, byte note, byte velocity) {
    Serial.print(" - Incoming NoteOff from channel: ");
    Serial.print(channel);
    Serial.print(" - Note: ");
    Serial.print(note);
    Serial.print(" - Velocity: ");
    Serial.print(velocity);
    Serial.println();
  });
}

MCMidi MC_Midi;
