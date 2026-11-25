#ifndef VAD_H
#define VAD_H

#include <Arduino.h>
#include "protocol_manager.h"
#include "sound.h"
#include "config.h"

class VAD {
public:
  using OnStopListening = void (*)();

  VAD(ProtocolManager& proto, Sound& sound) : proto(proto), sound(sound) {}

  void begin(OnStopListening on_stop);
  void resetTimeout();
  void tick();

private:
  static int16_t energy(const int16_t* samples, size_t count);

  static constexpr size_t CHUNK_SAMPLES = 512;
  static constexpr size_t PREROLL_CHUNKS = 4; // ~128ms of audio before the trigger

  ProtocolManager& proto;
  Sound& sound;
  OnStopListening onStop = nullptr;
  unsigned long lastVoiceTime = 0;
  bool sending = false;
  size_t prerollIndex = 0;
  int prerollBytes[PREROLL_CHUNKS] = {0};
  int16_t preroll[PREROLL_CHUNKS][CHUNK_SAMPLES];
  int16_t samples[CHUNK_SAMPLES];
};

#endif
