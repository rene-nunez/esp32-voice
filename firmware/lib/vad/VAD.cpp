#include "VAD.h"
#include "config.h"

void VAD::begin(OnStopListening on_stop) {
  onStop = on_stop;
}

void VAD::resetTimeout() {
  lastVoiceTime = 0;
  sending = false;
  for (size_t i = 0; i < PREROLL_CHUNKS; i++) {
    prerollBytes[i] = 0;
  }
}

int16_t VAD::energy(const int16_t* samples, size_t count) {
  int32_t sum = 0;
  for (size_t i = 0; i < count; i++) {
    int32_t val = samples[i];
    if (val < 0) val = -val;
    sum += val;
  }
  return (int16_t)(sum / count);
}

void VAD::tick() {
  int bytes_read = sound.readMic(samples, CHUNK_SAMPLES);

  if (bytes_read > 0) {
    int count = bytes_read / (int)sizeof(int16_t);
    int16_t e = energy(samples, count);

    if (e > VAD_ENERGY_THRESHOLD) {
      lastVoiceTime = millis();
    }

    if (lastVoiceTime > 0) {
      if (!sending) {
        sending = true;
        for (size_t i = 0; i < PREROLL_CHUNKS; i++) {
          size_t idx = (prerollIndex + i) % PREROLL_CHUNKS;
          if (prerollBytes[idx] > 0) {
            proto.sendAudio(preroll[idx], prerollBytes[idx]);
          }
        }
      }

      proto.sendAudio(samples, bytes_read);

      if (millis() - lastVoiceTime > SILENCE_TIMEOUT_MS) {
        Serial.println("[vad] silence timeout");
        proto.sendControl("VOICE_END");
        if (onStop) onStop();
        sending = false;
        lastVoiceTime = 0;
      }
    } else {
      prerollBytes[prerollIndex] = bytes_read;
      memcpy(preroll[prerollIndex], samples, bytes_read);
      prerollIndex = (prerollIndex + 1) % PREROLL_CHUNKS;
    }
  }
}
