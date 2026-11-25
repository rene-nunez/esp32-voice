#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "network.h"

class Protocol {
public:
  explicit Protocol(Network& net) : net(net) {}

  void sendAudio(const int16_t* samples, size_t bytes);
  void sendControl(const char* command);

private:
  void send(uint8_t type, const uint8_t* payload, size_t len);

  Network& net;
  uint8_t buf[1030];
};

#endif
