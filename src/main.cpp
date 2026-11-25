#include <WiFi.h>
#include <esp_system.h>

#include "config.h"

#include "network.h"
#include "protocol.h"
#include "sound.h"
#include "button.h"
#include "vad.h"

#ifndef WIFI_SSID
  #error "WIFI_SSID not defined. Copy include/secrets.h.example to include/secrets.h"
#endif

static Network         net;
static Protocol      proto(net);
static Sound           sound;
static Button          button(proto);
static VAD             vad(proto, sound);

static void onTextMessage(const String& text) {
  if (text.startsWith("[log] ")) {
    Serial.println(text.substring(6));
  } else if (text.startsWith("LANG:")) {
    sound.setTtsLang(text.substring(5));
  } else {
    sound.ttsQueue().enqueue(text);
  }
}

static void onStartListening() {
  vad.resetTimeout();
}

static void onStopListening() {
  button.stopListening();
}

static void printResetReason() {
  switch (esp_reset_reason()) {
    case ESP_RST_BROWNOUT: Serial.println("[sys] reset: brownout");  break;
    case ESP_RST_POWERON:  Serial.println("[sys] reset: power-on");  break;
    case ESP_RST_SW:       Serial.println("[sys] reset: software");  break;
    case ESP_RST_PANIC:    Serial.println("[sys] reset: panic");     break;
    case ESP_RST_WDT:      Serial.println("[sys] reset: watchdog");  break;
    default: break;
  }
}

void setup() {
  Serial.begin(115200);
  printResetReason();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("[sys] wifi connecting...");
  int timeout = 30;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(500);
    timeout--;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[sys] wifi timeout, restarting");
    ESP.restart();
  }

  Serial.print("[sys] wifi ");
  Serial.println(WiFi.localIP());

  delay(100);

  net.begin(onTextMessage, nullptr);
  sound.initSpeaker();
  sound.initMic();
  button.begin(onStartListening);
  vad.begin(onStopListening);

  Serial.println("[sys] ready");
}

void loop() {
  net.tick();
  sound.tick();

  if (WiFi.status() != WL_CONNECTED) return;
  if (sound.isPlaying()) return;

  button.tick();

  if (button.isListening()) {
    vad.tick();
  } else {
    yield();
  }
}
