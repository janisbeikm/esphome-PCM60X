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

 protected:
  void send_command_(const std::string &command);
  std::string receive_response_();
  uint16_t calculate_crc_(const char *data, size_t length);
  void parse_qpigs_(const std::string &data);
  void parse_qpiri_(const std::string &data);

  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *pv_voltage_sensor_{nullptr};
  sensor::Sensor *charging_current_sensor_{nullptr};
};

}  // namespace pcm60x
}  // namespace esphome
