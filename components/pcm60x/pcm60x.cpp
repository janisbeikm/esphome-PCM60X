#include "pcm60x.h"
#include "esphome/core/log.h"
#include <sstream>
#include <vector>
#include <cstdlib>  // for strtof()
#include <iomanip>  // for hex formatting

namespace esphome {
namespace pcm60x {

static const char *const TAG = "pcm60x";

void PCM60XComponent::setup() {
  ESP_LOGI(TAG, "Initializing PCM60X.");
}

void PCM60XComponent::update() {
  static std::vector<std::string> commands = {
    "QPI", "QID", "QVFW", "QPIRI", "QPIGS", "QDI", "QPIWS", "QBEQI"
  };
  static size_t cmd_index = 0;

  const std::string &cmd = commands[cmd_index];
  ESP_LOGD(TAG, "Running update(), sending %s", cmd.c_str());
  this->send_command_(cmd);
  std::string response = this->receive_response_();

  if (!response.empty()) {
    ESP_LOGD(TAG, "Raw response to %s: %s", cmd.c_str(), response.c_str());
    if (cmd == "QPIGS") {
      this->parse_qpigs_(response);
    } else if (cmd == "QPIRI") {
      this->parse_qpiri_(response);
    }
    // Future: Add other parsers for QPI, QDI, etc.
  } else {
    ESP_LOGW(TAG, "No response received for %s", cmd.c_str());
  }

  cmd_index = (cmd_index + 1) % commands.size();
}

void PCM60XComponent::send_command_(const std::string &command) {
  if (command == "QPIGS") {
    ESP_LOGD(TAG, "[DEBUG TEST] QPIGS calculated CRC should be 0xB7A9!");
  }

  const char* raw = command.c_str();
  size_t len = command.length();

  ESP_LOGD(TAG, "Command string length: %d", static_cast<int>(len));
  for (size_t i = 0; i < len; i++) {
    ESP_LOGD(TAG, "Char %d = 0x%02X (%c)", static_cast<int>(i), (uint8_t)raw[i], isprint(raw[i]) ? raw[i] : '.');
  }

  uint16_t crc = this->calculate_crc_(raw, len);
  ESP_LOGD(TAG, "Calculated CRC: 0x%04X", crc);

  std::string full_command = command;
  full_command += static_cast<char>(crc >> 8);  // high byte first for PCM60X
  full_command += static_cast<char>(crc & 0xFF);  // low byte
  full_command += '\r';

  ESP_LOGD(TAG, "Sending command bytes:");
  std::ostringstream debug_stream;
  debug_stream << "Sent command: ";
  for (size_t i = 0; i < full_command.size(); ++i) {
    uint8_t byte = static_cast<uint8_t>(full_command[i]);
    ESP_LOGD(TAG, "Byte %d: 0x%02X (%c)", (int)i, byte,
             isprint(byte) ? byte : '.');
    this->write(byte);

    debug_stream << std::hex << std::uppercase << std::setfill('0')
                 << "0x" << std::setw(2) << static_cast<int>(byte) << " ";
  }
  ESP_LOGD(TAG, "%s", debug_stream.str().c_str());
}

std::string PCM60XComponent::receive_response_() {
  while (this->available()) {
    this->read();  // flush leftovers
  }

  std::string result;
  unsigned long start = millis();
  while (millis() - start < 500) {
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

uint16_t PCM60XComponent::calculate_crc_(const char* data, size_t length) {
  uint16_t crc = 0x0000;

  for (size_t i = 0; i < length; i++) {
    crc ^= (static_cast<uint8_t>(data[i]) << 8);
    for (int j = 0; j < 8; j++) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ 0x1021;
      } else {
        crc <<= 1;
      }
      crc &= 0xFFFF;  // trim to 16 bits
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
