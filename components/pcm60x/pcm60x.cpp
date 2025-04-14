#include "pcm60x.h"
#include "esphome/core/log.h"
#include <sstream>
#include <vector>
#include <cstdlib>  // for strtof()

namespace esphome {
namespace pcm60x {

static const char *const TAG = "pcm60x";

void PCM60XComponent::setup() {
  ESP_LOGI(TAG, "Initializing PCM60X.");
}

void PCM60XComponent::update() {
  static std::vector<std::string> commands = {
    "QPIRI", "QPIGS", "QDI", "QPIWS", "QBEQI"
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
      else if (cmd == "QDI") {
      this->parse_qdi_(response);
    }
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
  full_command += static_cast<char>(crc >> 8);
  full_command += static_cast<char>(crc & 0xFF);
  full_command += '\r';

  ESP_LOGD(TAG, "Sending command bytes:");
  for (size_t i = 0; i < full_command.size(); ++i) {
    ESP_LOGD(TAG, "Byte %d: 0x%02X (%c)", (int)i, (uint8_t)full_command[i],
              isprint(full_command[i]) ? full_command[i] : '.');
    this->write((uint8_t)full_command[i]);
  }

  std::ostringstream hex_stream;
  for (size_t i = 0; i < full_command.size(); ++i) {
    hex_stream << "0x" << std::hex << std::uppercase << (int)(uint8_t)full_command[i] << " ";
  }
  ESP_LOGD(TAG, "Sent command: %s", hex_stream.str().c_str());
}

std::string PCM60XComponent::receive_response_() {
  while (this->available()) {
    this->read();
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
      crc &= 0xFFFF;
    }
  }
  return crc;
}

void PCM60XComponent::parse_qpigs_(const std::string &data) {
  std::string cleaned_data = data;
  if (!cleaned_data.empty() && cleaned_data[0] == '(') {
    cleaned_data = cleaned_data.substr(1);
  }

  std::vector<std::string> parts;
  std::string token;
  std::istringstream stream(cleaned_data);
  while (std::getline(stream, token, ' ')) {
    parts.push_back(token);
  }

  ESP_LOGD(TAG, "QPIGS part count: %d", parts.size());

  if (parts.size() < 11) {
    ESP_LOGW(TAG, "QPIGS response too short, got %d parts", parts.size());
    return;
  }

  float pv_voltage = std::strtof(parts[0].c_str(), nullptr);
  float battery_voltage = std::strtof(parts[1].c_str(), nullptr);
  float charging_current = std::strtof(parts[2].c_str(), nullptr);
  float charging_current1 = std::strtof(parts[3].c_str(), nullptr);
  float charging_current2 = std::strtof(parts[4].c_str(), nullptr);
  int charging_power = std::atoi(parts[5].c_str());
  int temperature = std::atoi(parts[6].c_str());
  float remote_batt_voltage = std::strtof(parts[7].c_str(), nullptr);
  int remote_batt_temp = std::atoi(parts[8].c_str());

  ESP_LOGD(TAG, "QPIGS decoded:");
  ESP_LOGD(TAG, "PV Input Voltage: %.1f V", pv_voltage);
  ESP_LOGD(TAG, "Battery Voltage: %.2f V", battery_voltage);
  ESP_LOGD(TAG, "Charging Current: %.2f A", charging_current);
  ESP_LOGD(TAG, "Charging Current 1: %.2f A", charging_current1);
  ESP_LOGD(TAG, "Charging Current 2: %.2f A", charging_current2);
  ESP_LOGD(TAG, "Charging Power: %d W", charging_power);
  ESP_LOGD(TAG, "Unit Temperature: %d °C", temperature);
  ESP_LOGD(TAG, "Remote Battery Voltage: %.2f V", remote_batt_voltage);
  ESP_LOGD(TAG, "Remote Battery Temperature: %d °C", remote_batt_temp);

  if (this->pv_voltage_sensor_ != nullptr)
    this->pv_voltage_sensor_->publish_state(pv_voltage);
  if (this->battery_voltage_sensor_ != nullptr)
    this->battery_voltage_sensor_->publish_state(battery_voltage);
  if (this->charging_current_sensor_ != nullptr)
    this->charging_current_sensor_->publish_state(charging_current);
}

void PCM60XComponent::parse_qpiri_(const std::string &data) {
  std::string cleaned_data = data;
  if (!cleaned_data.empty() && cleaned_data[0] == '(') {
    cleaned_data = cleaned_data.substr(1);
  }

  std::vector<std::string> parts;
  std::string token;
  std::istringstream stream(cleaned_data);
  while (std::getline(stream, token, ' ')) {
    parts.push_back(token);
  }
  ESP_LOGD(TAG, "QPIRI part count: %d", parts.size());
  if (parts.size() < 13) {
    ESP_LOGW(TAG, "QPIRI response too short, got %d parts", parts.size());
    return;
  }

  ESP_LOGD(TAG, "QPIRI Response: %s", data.c_str());

  long max_output_power = std::strtol(parts[0].c_str(), nullptr, 10);
  float nominal_battery_voltage = std::strtof(parts[1].c_str(), nullptr);
  float nominal_charging_current = std::strtof(parts[2].c_str(), nullptr);
  float absorption_voltage = std::strtof(parts[3].c_str(), nullptr);
  float float_voltage = std::strtof(parts[4].c_str(), nullptr);
  int battery_type = std::atoi(parts[5].c_str());
  int remote_battery_voltage_detect = std::atoi(parts[6].c_str());
  float temp_compensation = std::strtof(parts[7].c_str(), nullptr);
  int remote_temp_detect = std::atoi(parts[8].c_str());
  int battery_rated_voltage_set = std::atoi(parts[9].c_str());
  float batteries_in_series = std::strtof(parts[10].c_str(), nullptr);
  float low_warning_voltage = std::strtof(parts[11].c_str(), nullptr);
  int low_shutdown_detect = std::atoi(parts[12].c_str());

  const char* battery_type_str = battery_type == 0 ? "AGM" :
                                 battery_type == 1 ? "Flooded" :
                                 battery_type == 2 ? "User" : "Unknown";

  const char* remote_batt_str = remote_battery_voltage_detect == 1 ? "Yes" : "No";
  const char* temp_detect_str = remote_temp_detect == 1 ? "Yes" : "No";

  const char* rated_voltage_str = battery_rated_voltage_set == 4 ? "48V" :
                                  battery_rated_voltage_set == 2 ? "24V" :
                                  battery_rated_voltage_set == 1 ? "12V" : "Unknown";

  const char* shutdown_str = low_shutdown_detect == 1 ? "Enabled" : "Disabled";

  ESP_LOGD(TAG, "QPIRI decoded:");
  ESP_LOGD(TAG, "Max Output Power: %ld W", max_output_power);
  ESP_LOGD(TAG, "Nominal Battery Voltage: %.1f V", nominal_battery_voltage);
  ESP_LOGD(TAG, "Nominal Charging Current: %.1f A", nominal_charging_current);
  ESP_LOGD(TAG, "Absorption Voltage: %.2f V", absorption_voltage);
  ESP_LOGD(TAG, "Float Voltage: %.2f V", float_voltage);
  ESP_LOGD(TAG, "Battery Type: %s", battery_type_str);
  ESP_LOGD(TAG, "Remote Batt Voltage Detect: %s", remote_batt_str);
  ESP_LOGD(TAG, "Temp Compensation: %.1f mV/unit/°C", temp_compensation);
  ESP_LOGD(TAG, "Remote Temp Detect: %s", temp_detect_str);
  ESP_LOGD(TAG, "Battery Rated V Set: %s", rated_voltage_str);
  ESP_LOGD(TAG, "Batteries in Series: %.0f", batteries_in_series);
  ESP_LOGD(TAG, "Low Warning Voltage: %.2f V", low_warning_voltage);
  ESP_LOGD(TAG, "Low Shutdown Detect: %s", shutdown_str);
}

void PCM60XComponent::parse_qdi_(const std::string &data) {
  std::string cleaned = data;
  if (!cleaned.empty() && cleaned[0] == '(') cleaned = cleaned.substr(1);

  std::vector<std::string> parts;
  std::istringstream stream(cleaned);
  std::string token;
  while (std::getline(stream, token, ' ')) parts.push_back(token);

  ESP_LOGD(TAG, "QDI part count: %d", parts.size());

  if (parts.size() < 8) {
    ESP_LOGW(TAG, "QDI response too short, got %d parts", parts.size());
    return;
  }

  const char *battery_voltage_set_str = "Unknown";
  if (parts[0] == "00") battery_voltage_set_str = "Auto";
  else if (parts[0] == "01") battery_voltage_set_str = "12V";
  else if (parts[0] == "02") battery_voltage_set_str = "24V";
  else if (parts[0] == "03") battery_voltage_set_str = "36V";
  else if (parts[0] == "04") battery_voltage_set_str = "48V";

  const char *battery_type_str = "Unknown";
  if (parts[2] == "00") battery_type_str = "AGM";
  else if (parts[2] == "01") battery_type_str = "Flooded";
  else if (parts[2] == "02") battery_type_str = "User";

  int max_charging_current = std::atoi(parts[1].c_str());
  float absorption_voltage = std::strtof(parts[3].c_str(), nullptr);
  float float_voltage = std::strtof(parts[4].c_str(), nullptr);
  const char *remote_voltage_detect_str = (parts[5] == "01") ? "No" : "Yes";
  float temp_comp = std::strtof(parts[6].c_str(), nullptr);

  ESP_LOGD(TAG, "QDI decoded:");
  ESP_LOGD(TAG, "Battery Voltage Set: %s", battery_voltage_set_str);
  ESP_LOGD(TAG, "Max Charging Current: %d A", max_charging_current);
  ESP_LOGD(TAG, "Battery Type: %s", battery_type_str);
  ESP_LOGD(TAG, "Absorption Voltage: %.2f V", absorption_voltage);
  ESP_LOGD(TAG, "Float Voltage: %.2f V", float_voltage);
  ESP_LOGD(TAG, "Remote Batt Voltage Detect: %s", remote_voltage_detect_str);
  ESP_LOGD(TAG, "BTS Temp Compensation: %.1f mV/°C", temp_comp);
}
}  // namespace pcm60x
}  // namespace esphome
