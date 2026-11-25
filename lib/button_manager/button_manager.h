#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "protocol_manager.h"
#include "config.h"

class ButtonManager {
public:
  using OnStartListening = void (*)();

  explicit ButtonManager(ProtocolManager& proto) : proto(proto) {}

  void begin(OnStartListening on_start);
  void tick();
  bool isListening() const;
  void stopListening();

private:
  void handlePress();

  ProtocolManager& proto;
  OnStartListening onStart = nullptr;
  bool listening = false;
  bool debounced = false;
  bool pending = false;
  unsigned long pendingSince = 0;
};

#endif
