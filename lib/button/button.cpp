#include "button.h"
#include "pins.h"

void Button::begin(OnStartListening on_start) {
  onStart = on_start;
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

void Button::tick() {
  if (millis() < 2000) return;

  bool btn_now = (digitalRead(PIN_BTN) == LOW);

  if (btn_now == pending) {
    if (btn_now != debounced && millis() - pendingSince >= DEBOUNCE_MS) {
      debounced = btn_now;
      if (!btn_now) handlePress();
    }
  } else {
    pending = btn_now;
    pendingSince = millis();
  }
}

void Button::handlePress() {
  listening = !listening;
  digitalWrite(PIN_LED, listening);

  if (listening) {
    if (onStart) onStart();
    proto.sendControl("VOICE_START");
    Serial.println("[btn] listening on");
  } else {
    proto.sendControl("VOICE_END");
    Serial.println("[btn] listening off");
  }
}

bool Button::isListening() const {
  return listening;
}

void Button::stopListening() {
  listening = false;
  digitalWrite(PIN_LED, LOW);
}
