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
  ESP_LOGD(TAG, "Running update(), sending QPIGS111");

  this->send_command_("QPIGS");
  delay(100);  // allow time for response
  std::string response = this->receive_response_();
  if (!response.empty()) {
    this->parse_qpigs_(response);
  }

  this->send_command_("QPIRI");
  delay(100);  // allow time for response
  response = this->receive_response_();
  if (!response.empty()) {
    this->parse_qpiri_(response);
  }
}

void PCM60XComponent::send_command_(const std::string &command) {
  uint16_t crc = this->calculate_crc_(command);

  ESP_LOGD(TAG, "Running send_command_ for: %s", command.c_str());
  ESP_LOGD(TAG, "Calculated CRC: 0x%04X", crc);

  std::string full_command = command;
  full_command += static_cast<char>(crc & 0xFF);         // low byte
  full_command += static_cast<char>((crc >> 8) & 0xFF);  // high byte
  full_command += '\r';

  ESP_LOGD(TAG, "Sending command bytes:");
  for (size_t i = 0; i < full_command.size(); ++i) {
    ESP_LOGD(TAG, "Byte %d: 0x%02X (%c)", (int)i, (uint8_t)full_command[i],
              isprint(full_command[i]) ? full_command[i] : '.');
    this->write((uint8_t)full_command[i]);
  }
}

std::string PCM60XComponent::receive_response_() {
  // flush any leftover bytes
  while (this->available()) {
    this->read();
  }

  std::string result;
  unsigned long start = millis();
  while (millis() - start < 500) {  // timeout after 500ms
    while (this->available()) {
      char c = this->read();
      if (c == '\r') {
        ESP_LOGD(TAG, "Raw response: %s", result.c_str());
        return result;
      }
      result += c;
    }
    delay(10);
  }

  ESP_LOGW(TAG, "No response received within timeout");
  return "";
}

uint16_t PCM60XComponent::calculate_crc_(const std::string &data) {
  uint16_t crc = 0;
  for (size_t i = 0; i < data.size(); i++) {
    crc ^= static_cast<uint8_t>(data[i]);
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
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
