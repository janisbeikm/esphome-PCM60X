#include "pcm60x.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pcm60x {

static const char *const TAG = "pcm60x";

void PCM60XComponent::setup() {
  ESP_LOGI(TAG, "Initializing PCM60X...");
}

void PCM60XComponent::loop() {
  this->send_command_("QPIGS");
  std::string response = this->receive_response_();
  if (!response.empty()) {
    ESP_LOGD(TAG, "Received: %s", response.c_str());
  }
  delay(5000);  // avoid hammering the serial port
}

void PCM60XComponent::send_command_(const std::string &command) {
  uint16_t crc = calculate_crc_(command);
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%02X", crc);
  std::string full_command = command + buffer + "\r";
  this->write_str(full_command.c_str());
}

std::string PCM60XComponent::receive_response_() {
  std::string result;
  while (this->available()) {
    char c = this->read();
    if (c == '\r') break;
    result += c;
  }
  return result;
}

// CRC-16/IBM (Modbus-style)
uint16_t PCM60XComponent::calculate_crc_(const std::string &data) {
  uint16_t crc = 0;
  for (char c : data) {
    crc ^= static_cast<uint8_t>(c);
    for (int i = 0; i < 8; i++) {
      if (crc & 0x0001)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc = crc >> 1;
    }
  }
  return crc;
}

}  // namespace pcm60x
}  // namespace esphome
