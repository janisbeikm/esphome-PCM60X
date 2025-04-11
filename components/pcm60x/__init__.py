import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import CONF_ID, UNIT_VOLT, UNIT_AMPERE, DEVICE_CLASS_VOLTAGE, ICON_FLASH

CODEOWNERS = ["@janisbeikm"]
DEPENDENCIES = ["uart", "sensor"]

pcm60x_ns = cg.esphome_ns.namespace("pcm60x")
PCM60XComponent = pcm60x_ns.class_("PCM60XComponent", cg.Component, uart.UARTDevice)

CONF_PV_VOLTAGE = "pv_voltage"
CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_CHARGING_CURRENT = "charging_current"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PCM60XComponent),
    cv.Optional(CONF_PV_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        icon=ICON_FLASH,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    cv.Optional(CONF_CHARGING_CURRENT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        icon="mdi:current-dc",
    ),
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_PV_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_PV_VOLTAGE])
        cg.add(var.set_pv_voltage_sensor(sens))

    if CONF_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_VOLTAGE])
        cg.add(var.set_battery_voltage_sensor(sens))

    if CONF_CHARGING_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_CURRENT])
        cg.add(var.set_charging_current_sensor(sens))