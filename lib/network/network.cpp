#include "network.h"

#ifndef SERVER_IP
  #error "SERVER_IP not defined. Copy include/secrets.h.example to include/secrets.h"
#endif

#ifndef WS_PORT
  #define WS_PORT 8765
#endif

#define STRINGIFY_(x) #x
#define TOSTRING_(x) STRINGIFY_(x)
#define WS_URL "ws://" TOSTRING_(SERVER_IP) ":" TOSTRING_(WS_PORT)

bool Network::connect() {
  if (client.connect(WS_URL)) {
    Serial.println("[net] ws ok");
    return true;
  }
  return false;
}

void Network::begin(MessageCallback on_text, BinaryCallback on_binary) {
  onText = on_text;
  onBinary = on_binary;

  client.onMessage([this](websockets::WebsocketsMessage msg) {
    if (msg.isText()) {
      String text = msg.data();
      text.trim();
      if (text.length() > 0 && onText) {
        onText(text);
      }
    } else if (msg.isBinary()) {
      if (onBinary) {
        onBinary(
          (const uint8_t*)msg.data().c_str(),
          msg.data().length()
        );
      }
    }
  });

  client.onEvent([](websockets::WebsocketsEvent event, String) {
    if (event == websockets::WebsocketsEvent::ConnectionClosed) {
      Serial.println("[net] ws disconnected");
    }
  });

  if (!connect()) {
    Serial.println("[net] ws unavailable, retrying");
  }
}

void Network::tick() {
  if (client.available()) {
    client.poll();
    lastAttempt = 0;
  } else if (lastAttempt == 0 || millis() - lastAttempt > WS_RETRY_MS) {
    lastAttempt = millis();
    Serial.println("[net] ws reconnect");
    connect();
  }
}

bool Network::sendBinary(const uint8_t* data, size_t len) {
  return client.sendBinary((const char*)data, len);
}
