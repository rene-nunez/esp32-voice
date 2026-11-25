#include "Sound.h"
#include "pins.h"

static Sound* instance = nullptr;

void audio_eof_speech(const char*) {
  if (instance) instance->stopPlaying();
}

void audio_eof_mp3(const char*) {
  if (instance) instance->stopPlaying();
}

void audio_eof_stream(const char*) {
  if (instance) instance->stopPlaying();
}

void Sound::initSpeaker() {
  instance = this;
  player.setPinout(AMP_BCLK, AMP_LRC, AMP_DOUT);
  player.setVolume(24);
  Serial.println("[audio] speaker ok");
}

void Sound::initMic() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = true
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD
  };

  i2s_driver_install(I2S_MIC, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_MIC, &pin_config);
  i2s_set_clk(I2S_MIC, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void Sound::tick() {
  player.loop();

  if (!playing && !queue.empty()) {
    playing = true;
    playStart = millis();
    String payload = queue.dequeue();

    player.connecttospeech(payload.c_str(), ttsLang.c_str());
  }

  if (playing && (millis() - playStart > PLAY_TIMEOUT)) {
    Serial.println("[audio] playback timeout");
    player.stopSong();
    playing = false;
  }
}

bool Sound::isPlaying() const {
  return playing;
}

void Sound::stopPlaying() {
  playing = false;
}

void Sound::setTtsLang(const String& lang) {
  ttsLang = lang;
}

int Sound::readMic(int16_t* samples, size_t max_samples) {
  size_t bytes_read = 0;
  i2s_read(I2S_MIC, samples, max_samples * sizeof(int16_t), &bytes_read, pdMS_TO_TICKS(10));
  return (int)bytes_read;
}
