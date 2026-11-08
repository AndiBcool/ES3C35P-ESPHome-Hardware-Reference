#pragma once
// Guinea Buddy v1.5: ESPHome microphone/speaker only; no legacy I2S driver.
#include "esphome/components/microphone/microphone.h"
#include "esphome/components/speaker/speaker.h"
#include "esphome/components/audio/audio.h"
#include "esphome/core/log.h"
#include <freertos/FreeRTOS.h>
#include <freertos/stream_buffer.h>
#include <freertos/task.h>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace guinea_native {
static constexpr const char *TAG = "guinea_native";
static constexpr const char *PATH = "/sdcard/aufnahme.wav";
static StreamBufferHandle_t fifo = nullptr;
static FILE *record_file = nullptr;
static uint32_t record_bytes = 0;
// Erste 100 ms bei 48 kHz, 16-bit mono verwerfen: 9600 PCM-Bytes.
static std::atomic<uint32_t> startup_skip_bytes{0};
static std::atomic<uint32_t> dropped_bytes{0};
static std::atomic<bool> recording{false};
static std::atomic<bool> playing{false};
static std::atomic<bool> play_done{true};
static std::atomic<uint32_t> played_bytes{0};
static esphome::speaker::Speaker *output = nullptr;

inline void put16(FILE *f, uint16_t v) {
  uint8_t b[2] = {static_cast<uint8_t>(v), static_cast<uint8_t>(v >> 8)};
  fwrite(b, 1, 2, f);
}
inline void put32(FILE *f, uint32_t v) {
  uint8_t b[4] = {static_cast<uint8_t>(v), static_cast<uint8_t>(v >> 8),
                  static_cast<uint8_t>(v >> 16), static_cast<uint8_t>(v >> 24)};
  fwrite(b, 1, 4, f);
}
inline void wav_header(FILE *f, uint32_t n) {
  fseek(f, 0, SEEK_SET);
  fwrite("RIFF", 1, 4, f); put32(f, n + 36);
  fwrite("WAVEfmt ", 1, 8, f); put32(f, 16);
  put16(f, 1); put16(f, 1); put32(f, 48000); put32(f, 96000);
  put16(f, 2); put16(f, 16);
  fwrite("data", 1, 4, f); put32(f, n);
}
inline void init(esphome::microphone::Microphone *mic, esphome::speaker::Speaker *spk) {
  output = spk;
  if (!fifo) fifo = xStreamBufferCreate(65536, 1);
  if (!fifo) { ESP_LOGE(TAG, "Kein RAM fuer Mikrofonpuffer"); return; }
  mic->add_data_callback([](const std::vector<uint8_t> &data) {
    if (!recording.load() || !fifo || data.empty()) return;
    size_t offset = 0;
    const uint32_t remaining = startup_skip_bytes.load();
    if (remaining) {
      offset = std::min(static_cast<size_t>(remaining), data.size());
      startup_skip_bytes.store(remaining - static_cast<uint32_t>(offset));
    }
    if (offset == data.size()) return;
    const size_t wanted = data.size() - offset;
    size_t n = xStreamBufferSend(fifo, data.data() + offset, wanted, 0);
    if (n < wanted) dropped_bytes.fetch_add(wanted - n);
  });
  ESP_LOGI(TAG, "Native Mikrofon-Callback registriert");
}
inline bool begin_record() {
  if (recording.load() || playing.load() || !fifo) {
    ESP_LOGW(TAG, "Aufnahme derzeit nicht moeglich"); return false;
  }
  record_file = fopen(PATH, "wb");
  if (!record_file) { ESP_LOGE(TAG, "WAV kann nicht geoeffnet werden: %s", PATH); return false; }
  wav_header(record_file, 0);
  xStreamBufferReset(fifo);
  record_bytes = 0;
  dropped_bytes.store(0);
  startup_skip_bytes.store(9600);  // 100 ms Einschwingphase nicht in WAV speichern
  recording.store(true);
  ESP_LOGI(TAG, "Aufnahme gestartet: %s", PATH);
  return true;
}
// Call every 20 ms from ESPHome interval. SD writes occur in main loop, never in audio callback.
inline void pump() {
  if (!record_file || !fifo) return;
  uint8_t buf[2048];
  for (int i = 0; i < 4; ++i) {
    size_t n = xStreamBufferReceive(fifo, buf, sizeof(buf), 0);
    if (!n) break;
    size_t w = fwrite(buf, 1, n, record_file);
    record_bytes += w;
    if (w != n) { ESP_LOGE(TAG, "SD Schreibfehler: %u/%u", (unsigned) w, (unsigned) n); break; }
  }
}
inline void finish_record() {
  if (!record_file) return;
  recording.store(false);
  // The caller must stop microphone and wait for is_stopped() before this call.
  pump();
  // Drain any remaining buffered data.
  while (fifo && xStreamBufferBytesAvailable(fifo)) pump();
  wav_header(record_file, record_bytes);
  fflush(record_file);
  fclose(record_file);
  record_file = nullptr;
  ESP_LOGI(TAG, "WAV gespeichert: %s, PCM=%u Bytes, verloren=%u Bytes",
           PATH, (unsigned) record_bytes, (unsigned) dropped_bytes.load());
}
inline void playback_task(void *) {
  FILE *f = fopen(PATH, "rb");
  if (!f) { ESP_LOGE(TAG, "WAV nicht gefunden"); playing.store(false); play_done.store(true); vTaskDelete(nullptr); return; }
  uint8_t header[44];
  if (fread(header, 1, 44, f) != 44 || memcmp(header, "RIFF", 4) || memcmp(header + 8, "WAVE", 4) ||
      memcmp(header + 36, "data", 4) || header[22] != 1 || header[23] != 0 ||
      header[24] != 0x80 || header[25] != 0xbb || header[26] || header[27] ||
      header[34] != 16 || header[35] != 0) {
    ESP_LOGE(TAG, "WAV Format ungueltig (erwartet PCM 48kHz mono 16bit)");
    fclose(f); playing.store(false); play_done.store(true); vTaskDelete(nullptr); return;
  }
  // Set stream metadata explicitly; otherwise ESPHome defaults to 16 kHz.
  output->set_audio_stream_info(esphome::audio::AudioStreamInfo(16, 1, 48000));
  ESP_LOGI(TAG, "Speaker-Stream gesetzt: PCM 48000 Hz, mono, 16 Bit");
  uint8_t buf[2048];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
    size_t pos = 0;
    int retries = 0;
    while (pos < n) {
      size_t sent = output->play(buf + pos, n - pos, pdMS_TO_TICKS(100));
      if (sent) { pos += sent; played_bytes.fetch_add(sent); retries = 0; }
      else if (++retries > 30) { ESP_LOGE(TAG, "Speaker nimmt keine PCM-Daten an"); goto done; }
      vTaskDelay(pdMS_TO_TICKS(5));
    }
  }
done:
  fclose(f);
  output->finish();
  ESP_LOGI(TAG, "WAV an nativen Speaker uebergeben: %u Bytes", (unsigned) played_bytes.load());
  playing.store(false);
  play_done.store(true);
  vTaskDelete(nullptr);
}
inline bool begin_play() {
  if (recording.load() || record_file || playing.load() || !output) {
    ESP_LOGW(TAG, "Wiedergabe derzeit nicht moeglich"); return false;
  }
  FILE *f = fopen(PATH, "rb");
  if (!f) { ESP_LOGE(TAG, "Keine Aufnahme vorhanden: %s", PATH); return false; }
  fclose(f);
  playing.store(true); play_done.store(false); played_bytes.store(0);
  if (xTaskCreate(playback_task, "guinea_wav_play", 6144, nullptr, 4, nullptr) != pdPASS) {
    playing.store(false); play_done.store(true);
    ESP_LOGE(TAG, "Playback Task konnte nicht starten"); return false;
  }
  ESP_LOGI(TAG, "Native WAV-Wiedergabe gestartet");
  return true;
}
} // namespace guinea_native
