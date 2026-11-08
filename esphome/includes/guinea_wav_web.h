#pragma once
// Guinea Buddy v1.7: read-only WAV listing and streaming via ESP-IDF HTTP server.
#include "esp_http_server.h"
#include "esphome/core/log.h"
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <string>

namespace guinea_web {
static constexpr const char *TAG = "guinea_web";
static httpd_handle_t server = nullptr;

inline bool valid_name(const char *s) {
  size_t n = strlen(s);
  if (n < 5 || n > 100 || strcmp(s + n - 4, ".wav") != 0) return false;
  for (size_t i = 0; i < n - 4; ++i) {
    char c = s[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
  }
  return true;
}

inline esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  httpd_resp_set_hdr(req, "Cache-Control", "no-store");
  const char *start = "<!doctype html><html lang='de'><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Guinea Buddy WAV</title><style>body{font:16px system-ui,sans-serif;"
    "max-width:720px;margin:32px auto;padding:0 18px;background:#f7f8fa;color:#202530}"
    "li{padding:13px;background:white;margin:8px 0;border-radius:9px;list-style:none}"
    "ul{padding:0}a{color:#1267a5}small{color:#667}</style></head><body>"
    "<h1>Guinea Buddy</h1><p>WAV-Aufnahmen auf der microSD</p>"
    "<p><a href='/'>Liste aktualisieren</a></p><ul>";
  httpd_resp_sendstr_chunk(req, start);
  // v1.7.1: The ESP-IDF FAT VFS on this device rejects directory/stat
  // queries in the HTTP task. The audio recorder uses fopen successfully.
  // Open the known WAV directly, without opendir() or stat().
  FILE *f = fopen(guinea_native::PATH, "rb");
  if (!f) {
    ESP_LOGW(TAG, "WAV kann nicht geoeffnet werden: %s", guinea_native::PATH);
    httpd_resp_sendstr_chunk(req, "<li>Keine lesbare aufnahme.wav vorhanden</li>");
  } else {
    long file_size = -1;
    if (fseek(f, 0, SEEK_END) == 0) file_size = ftell(f);
    fclose(f);
    char line[240];
    if (file_size >= 0) {
      snprintf(line, sizeof(line),
               "<li><a href='/download?file=aufnahme.wav'>aufnahme.wav herunterladen</a> "
               "<small>(%ld Bytes)</small></li>", file_size);
    } else {
      snprintf(line, sizeof(line),
               "<li><a href='/download?file=aufnahme.wav'>aufnahme.wav herunterladen</a></li>");
    }
    httpd_resp_sendstr_chunk(req, line);
    ESP_LOGI(TAG, "WAV-Datei per fopen erreichbar, Groesse=%ld", file_size);
  }
  httpd_resp_sendstr_chunk(req, "</ul><p><small>Nur Download. Kein Loeschen.</small></p></body></html>");
  return httpd_resp_sendstr_chunk(req, nullptr);
}

inline esp_err_t download_handler(httpd_req_t *req) {
  char query[160] = {};
  char name[110] = {};
  if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK ||
      httpd_query_key_value(query, "file", name, sizeof(name)) != ESP_OK ||
      !valid_name(name)) {
    httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Ungueltiger WAV-Dateiname");
    return ESP_FAIL;
  }
  // Never download a file that may currently be written or played.
  if (guinea_native::recording.load() || guinea_native::playing.load()) {
    httpd_resp_set_status(req, "503 Service Unavailable");
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, "Audio gerade aktiv; bitte erneut versuchen");
    return ESP_FAIL;
  }
  std::string path = std::string("/sdcard/") + name;
  FILE *f = fopen(path.c_str(), "rb");
  if (!f) {
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Datei nicht gefunden");
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "audio/wav");
  httpd_resp_set_hdr(req, "Cache-Control", "no-store");
  std::string disposition = std::string("attachment; filename=\"") + name + "\"";
  httpd_resp_set_hdr(req, "Content-Disposition", disposition.c_str());
  uint8_t buffer[2048];
  size_t n;
  esp_err_t result = ESP_OK;
  while ((n = fread(buffer, 1, sizeof(buffer), f)) > 0) {
    if (httpd_resp_send_chunk(req, reinterpret_cast<const char *>(buffer), n) != ESP_OK) {
      result = ESP_FAIL;
      break;
    }
    // Keep the SD card free when a recording is started during transfer.
    if (guinea_native::recording.load()) { result = ESP_FAIL; break; }
  }
  fclose(f);
  if (result == ESP_OK) httpd_resp_send_chunk(req, nullptr, 0);
  ESP_LOGI(TAG, "WAV Download %s: %s", name, result == ESP_OK ? "OK" : "abgebrochen");
  return result;
}

inline void start() {
  if (server) return;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 8080;
  config.max_uri_handlers = 4;
  config.stack_size = 6144;
  config.lru_purge_enable = true;
  esp_err_t err = httpd_start(&server, &config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "HTTP-Server konnte nicht starten: %s", esp_err_to_name(err));
    server = nullptr;
    return;
  }
  httpd_uri_t index_uri{};
  index_uri.uri = "/";
  index_uri.method = HTTP_GET;
  index_uri.handler = index_handler;
  httpd_uri_t download_uri{};
  download_uri.uri = "/download";
  download_uri.method = HTTP_GET;
  download_uri.handler = download_handler;
  httpd_register_uri_handler(server, &index_uri);
  httpd_register_uri_handler(server, &download_uri);
  ESP_LOGI(TAG, "WAV-Webserver gestartet, Port 8080 (nur lokales Netz)");
}
}  // namespace guinea_web
