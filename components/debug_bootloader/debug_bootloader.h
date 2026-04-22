#pragma once

#include "esphome/core/component.h"

namespace esphome::debug_bootloader {

class DebugBootloader : public Component {
 public:
  void setup() override;
  // void loop() override;
  void dump_config() override;
};


}  // namespace esphome::debug_bootloader