#include "protocol.h"
#include "config.h"

void Protocol::send(uint8_t type, const uint8_t* payload, size_t len) {
  buf[0] = type;
  buf[1] = (len >> 24) & 0xFF;
  buf[2] = (len >> 16) & 0xFF;
  buf[3] = (len >> 8)  & 0xFF;
  buf[4] = len & 0xFF;

  memcpy(buf + 5, payload, len);
  net.sendBinary(buf, 5 + len);
}

void Protocol::sendAudio(const int16_t* samples, size_t bytes) {
  send(MSG_AUDIO, (const uint8_t*)samples, bytes);
}

void Protocol::sendControl(const char* command) {
  send(MSG_TEXT, (const uint8_t*)command, strlen(command));
}
