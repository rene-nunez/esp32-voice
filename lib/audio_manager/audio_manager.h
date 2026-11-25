#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <Arduino.h>
#include <driver/i2s.h>

#include "Audio.h"
#include "ring_buffer.h"
#include "config.h"

class AudioManager {
public:
  void initSpeaker();
  void initMic();
  void tick();

  bool isPlaying() const;
  void stopPlaying();
  void setTtsLang(const String& lang);

  int readMic(int16_t* samples, size_t max_samples);

  RingBuffer<String, MAX_QUEUE>& ttsQueue() { return queue; }

private:
  Audio player;
  RingBuffer<String, MAX_QUEUE> queue;
  String ttsLang = "en";
  bool playing = false;
  unsigned long playStart = 0;
};

#endif
