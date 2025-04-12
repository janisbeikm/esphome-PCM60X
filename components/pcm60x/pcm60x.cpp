#include "pcm60x.h"
#include "esphome/core/log.h"
#include <sstream>
#include <vector>
#include <cstdlib>  // for strtof()

namespace esphome {
namespace pcm60x {

static const char *const TAG = "pcm60x";

void PCM60XComponent::setup() {
  ESP_LOGI(TAG, "Initializing PCM60X...");
}

void PCM60XComponent::update() {
  ESP_LOGD(TAG, "Running update(), sending QPIGS");
  this->send_command_("QPIGS");
  std::string response = this->receive_response_();
  if (!response.empty()) {
    this->parse_qpigs_(response);
  }

  this->send_command_("QPIRI");
  response = this->receive_response_();
  if (!response.empty()) {
    this->parse_qpiri_(response);
  }
}

void PCM60XComponent::send_command_(const std::string &command) {
  uint16_t crc = calculate_crc_(command);
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%02X", crc);
  std::string full_command = command + buffer + "\r";
  this->write_str(full_command.c_str());
  ESP_LOGD(TAG, "Sending command: %s", full_command.c_str());
}

std::string PCM60XComponent::receive_response_() {
  std::string result;
  while (this->available()) {
    char c = this->read();
    if (c == '\r') break;
    result += c;
  }
  ESP_LOGD(TAG, "Raw response: %s", result.c_str());
  return result;
  
}

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

void PCM60XComponent::parse_qpigs_(const std::string &data) {
  std::vector<std::string> parts;
  std::string token;
  std::istringstream stream(data);
  while (std::getline(stream, token, ' ')) {
    parts.push_back(token);
  }

  if (parts.size() < 3) {
    ESP_LOGW(TAG, "QPIGS response too short");
    return;
  }

  float pv_voltage = 0, battery_voltage = 0, charging_current = 0;

  char *end;
  pv_voltage = std::strtof(parts[0].c_str(), &end);
  battery_voltage = std::strtof(parts[1].c_str(), &end);
  charging_current = std::strtof(parts[2].c_str(), &end);

  if (this->pv_voltage_sensor_ != nullptr)
    this->pv_voltage_sensor_->publish_state(pv_voltage);
  if (this->battery_voltage_sensor_ != nullptr)
    this->battery_voltage_sensor_->publish_state(battery_voltage);
  if (this->charging_current_sensor_ != nullptr)
    this->charging_current_sensor_->publish_state(charging_current);

  ESP_LOGD(TAG, "QPIGS: PV=%.1fV, Bat=%.2fV, Current=%.2fA", pv_voltage, battery_voltage, charging_current);
}

void PCM60XComponent::parse_qpiri_(const std::string &data) {
  ESP_LOGD(TAG, "QPIRI Response: %s", data.c_str());
}

}  // namespace pcm60x
}  // namespace esphome
