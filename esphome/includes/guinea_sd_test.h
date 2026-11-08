#pragma once
#include "esphome/core/log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

namespace guinea_buddy {
static sdmmc_card_t *sd_card = nullptr;
static bool sd_mounted = false;
inline void sd_test_once() {
  static const char *TAG = "guinea_sd";
  if (sd_mounted) {
    ESP_LOGI(TAG, "microSD bereits eingebunden");
    return;
  }
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  host.slot = SDMMC_HOST_SLOT_1;
  host.max_freq_khz = 10000;
  sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
  slot.width = 4;
  slot.clk = GPIO_NUM_5;
  slot.cmd = GPIO_NUM_4;
  slot.d0 = GPIO_NUM_6;
  slot.d1 = GPIO_NUM_7;
  slot.d2 = GPIO_NUM_2;
  slot.d3 = GPIO_NUM_3;
  slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
  esp_vfs_fat_sdmmc_mount_config_t cfg = {};
  cfg.format_if_mount_failed = false;  // NEVER format the user's card
  cfg.max_files = 3;
  cfg.allocation_unit_size = 16 * 1024;
  esp_err_t err = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot, &cfg, &sd_card);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "microSD mount fehlgeschlagen: %s (0x%x)", esp_err_to_name(err), err);
    return;
  }
  sd_mounted = true;
  const double mb = (double) sd_card->csd.capacity * sd_card->csd.sector_size / (1024.0 * 1024.0);
  ESP_LOGI(TAG, "microSD erkannt: %.1f MiB, Sektor %u Bytes", mb, (unsigned) sd_card->csd.sector_size);
}
}  // namespace guinea_buddy
