#include "esphome/core/log.h"

#ifdef USE_ESP32
#include <esp_chip_info.h>
#include <esp_ota_ops.h>
#include <esp_bootloader_desc.h>
#endif

#include "debug_bootloader.h"

namespace esphome::debug_bootloader {

static const char *TAG = "debug_bootloader.component";

void DebugBootloader::setup() {

}

// void DebugBootloader::loop() {
//
// }

void DebugBootloader::dump_config(){
    ESP_LOGCONFIG(TAG, "Debug bootloader component");

#ifdef USE_ESP32
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "ESP32 Chip: %s rev%d.%d, %d core(s)", ESPHOME_VARIANT, chip_info.revision / 100,
             chip_info.revision % 100, chip_info.cores);
#if defined(USE_ESP32_VARIANT_ESP32) && (!defined(USE_ESP32_MIN_CHIP_REVISION_SET) || !defined(USE_ESP32_SRAM1_AS_IRAM))
    static const char *const ESP32_ADVANCED_PATH = "under esp32 > framework > advanced";
#endif
#if defined(USE_ESP32_VARIANT_ESP32) && !defined(USE_ESP32_MIN_CHIP_REVISION_SET)
    {
      // Suggest optimization for chips that don't need the PSRAM cache workaround
      if (chip_info.revision >= 300) {
#ifdef USE_PSRAM
        ESP_LOGW(TAG, "Chip rev >= 3.0 detected. Set minimum_chip_revision: \"%d.%d\" %s to save ~10KB IRAM",
                 chip_info.revision / 100, chip_info.revision % 100, ESP32_ADVANCED_PATH);
#else
        ESP_LOGW(TAG, "Chip rev >= 3.0 detected. Set minimum_chip_revision: \"%d.%d\" %s to reduce binary size",
                 chip_info.revision / 100, chip_info.revision % 100, ESP32_ADVANCED_PATH);
#endif
      }
    }
#endif
    {
      // esp_bootloader_desc_t is available in ESP-IDF >= 5.2; if readable the bootloader is modern.
      //
      // Design decision: We intentionally do NOT mention sram1_as_iram when the bootloader is too old.
      // Enabling sram1_as_iram with an old bootloader causes a hard brick (device fails to boot,
      // requires USB reflash to recover). Users don't always read warnings carefully, so we only
      // suggest the option once we've confirmed the bootloader can handle it. In practice this
      // means a user with an old bootloader may need to flash twice: once via USB to update the
      // bootloader (they'll see the suggestion on next boot), then OTA with sram1_as_iram: true.
      // Two flashes is a better outcome than a bricked device.
      esp_bootloader_desc_t boot_desc;
      if (esp_ota_get_bootloader_description(nullptr, &boot_desc) != ESP_OK) {
#ifdef USE_ESP32_VARIANT_ESP32
        ESP_LOGW(TAG, "Bootloader too old for OTA rollback and SRAM1 as IRAM (+40KB). "
                      "Flash via USB once to update the bootloader");
#else
        ESP_LOGW(TAG, "Bootloader too old for OTA rollback. Flash via USB once to update the bootloader");
#endif
      }
#if defined(USE_ESP32_VARIANT_ESP32) && !defined(USE_ESP32_SRAM1_AS_IRAM)
      else {
        ESP_LOGW(TAG, "Bootloader supports SRAM1 as IRAM (+40KB). Set sram1_as_iram: true %s", ESP32_ADVANCED_PATH);
      }
#endif
    }
#endif  // USE_ESP32
}


}  // namespace esphome::debug_bootloader