#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "Protocol.h"
#include "config.h"

class Button {
public:
  using OnStartListening = void (*)();

  explicit Button(Protocol& proto) : proto(proto) {}

  void begin(OnStartListening on_start);
  void tick();
  bool isListening() const;
  void stopListening();

private:
  void handlePress();

  Protocol& proto;
  OnStartListening onStart = nullptr;
  bool listening = false;
  bool debounced = false;
  bool pending = false;
  unsigned long pendingSince = 0;
};

#endif
