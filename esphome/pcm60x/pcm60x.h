#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace pcm60x {

class PCM60XComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;

 protected:
  void send_command_(const std::string &command);
  std::string receive_response_();
  uint16_t calculate_crc_(const std::string &data);
};

}  // namespace pcm60x
}  // namespace esphome
