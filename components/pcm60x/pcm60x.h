#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace pcm60x {

class PCM60XComponent : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void update() override;

  void set_battery_voltage_sensor(sensor::Sensor *sensor) { battery_voltage_sensor_ = sensor; }
  void set_pv_voltage_sensor(sensor::Sensor *sensor) { pv_voltage_sensor_ = sensor; }
  void set_charging_current_sensor(sensor::Sensor *sensor) { charging_current_sensor_ = sensor; }
  void set_charging_current_1_sensor(sensor::Sensor *s) { charging_current_1_sensor_ = s; }
  void set_charging_current_2_sensor(sensor::Sensor *s) { charging_current_2_sensor_ = s; }
  void set_charging_power_sensor(sensor::Sensor *s) { charging_power_sensor_ = s; }
  void set_unit_temperature_sensor(sensor::Sensor *s) { unit_temperature_sensor_ = s; }
  void set_remote_battery_voltage_sensor(sensor::Sensor *s) { remote_battery_voltage_sensor_ = s; }
  void set_remote_battery_temperature_sensor(sensor::Sensor *s) { remote_battery_temperature_sensor_ = s; }
  void set_max_output_power_sensor(sensor::Sensor *s) { max_output_power_sensor_ = s; }
  void set_nominal_battery_voltage_sensor(sensor::Sensor *s) { nominal_battery_voltage_sensor_ = s; }
  void set_nominal_charging_current_sensor(sensor::Sensor *s) { nominal_charging_current_sensor_ = s; }
  void set_absorption_voltage_sensor(sensor::Sensor *s) { absorption_voltage_sensor_ = s; }
  void set_float_voltage_sensor(sensor::Sensor *s) { float_voltage_sensor_ = s; }
  void set_temp_compensation_sensor(sensor::Sensor *s) { temp_compensation_sensor_ = s; }
  void set_batteries_in_series_sensor(sensor::Sensor *s) { batteries_in_series_sensor_ = s; }
  void set_low_warning_voltage_sensor(sensor::Sensor *s) { low_warning_voltage_sensor_ = s; }
  void set_battery_type_text(text_sensor::TextSensor *s) { battery_type_text_ = s; }
  void set_remote_batt_voltage_detect_text(text_sensor::TextSensor *s) { remote_batt_voltage_detect_text_ = s; }
  void set_remote_temp_detect_text(text_sensor::TextSensor *s) { remote_temp_detect_text_ = s; }
  void set_battery_rated_voltage_text(text_sensor::TextSensor *s) { battery_rated_voltage_text_ = s; }
  void set_low_shutdown_detect_text(text_sensor::TextSensor *s) { low_shutdown_detect_text_ = s; }
  void set_warning_status_bit_sensor(int index, binary_sensor::BinarySensor *sensor) {
    if (index >= 0 && index < 30) warning_status_bits_[index] = sensor;}
 protected:
  void send_command_(const std::string &command);
  std::string receive_response_();
  uint16_t calculate_crc_(const char *data, size_t length);
  void parse_qpigs_(const std::string &data);
  void parse_qpiri_(const std::string &data);
  void parse_qpiws_(const std::string &data);
  void parse_qbeqi_(const std::string &data);

  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *pv_voltage_sensor_{nullptr};
  sensor::Sensor *charging_current_sensor_{nullptr};
  sensor::Sensor *charging_current_1_sensor_{nullptr};
  sensor::Sensor *charging_current_2_sensor_{nullptr};
  sensor::Sensor *charging_power_sensor_{nullptr};
  sensor::Sensor *unit_temperature_sensor_{nullptr};
  sensor::Sensor *remote_battery_voltage_sensor_{nullptr};
  sensor::Sensor *remote_battery_temperature_sensor_{nullptr};
  sensor::Sensor *max_output_power_sensor_{nullptr};
  sensor::Sensor *nominal_battery_voltage_sensor_{nullptr};
  sensor::Sensor *nominal_charging_current_sensor_{nullptr};
  sensor::Sensor *absorption_voltage_sensor_{nullptr};
  sensor::Sensor *float_voltage_sensor_{nullptr};
  sensor::Sensor *temp_compensation_sensor_{nullptr};
  sensor::Sensor *batteries_in_series_sensor_{nullptr};
  sensor::Sensor *low_warning_voltage_sensor_{nullptr};
  text_sensor::TextSensor *battery_type_text_{nullptr};
  text_sensor::TextSensor *remote_batt_voltage_detect_text_{nullptr};
  text_sensor::TextSensor *remote_temp_detect_text_{nullptr};
  text_sensor::TextSensor *battery_rated_voltage_text_{nullptr};
  text_sensor::TextSensor *low_shutdown_detect_text_{nullptr};
  binary_sensor::BinarySensor *warning_status_bits_[30] = {nullptr};

};

}  // namespace pcm60x
}  // namespace esphome
