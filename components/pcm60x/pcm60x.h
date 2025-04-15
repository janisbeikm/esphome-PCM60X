#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

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

};

}  // namespace pcm60x
}  // namespace esphome
