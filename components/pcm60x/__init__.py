import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor, text_sensor, binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    ICON_FLASH,
    DEVICE_CLASS_PROBLEM,
)

CODEOWNERS = ["@janisbeikm"]
DEPENDENCIES = ["uart", "sensor", "text_sensor", "binary_sensor"]

pcm60x_ns = cg.esphome_ns.namespace("pcm60x")
PCM60XComponent = pcm60x_ns.class_("PCM60XComponent", cg.PollingComponent, uart.UARTDevice)

CONF_PV_VOLTAGE = "pv_voltage"
CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_CHARGING_CURRENT = "charging_current"
CONF_CHARGING_CURRENT1 = "charging_current_1"
CONF_CHARGING_CURRENT2 = "charging_current_2"
CONF_CHARGING_POWER = "charging_power"
CONF_UNIT_TEMPERATURE = "unit_temperature"
CONF_REMOTE_BATTERY_VOLTAGE = "remote_battery_voltage"
CONF_REMOTE_BATTERY_TEMPERATURE = "remote_battery_temperature"
CONF_MAX_OUTPUT_POWER = "max_output_powe"
CONF_NOMINAL_BATTERY_VOLTAGE = "nominal_battery_voltage"
CONF_NOMINAL_CHARGING_CURRENT = "nominal_charging_current"
CONF_ABSORPTION_VOLTAGE = "absorption_voltage"
CONF_FLOAT_VOLTAGE = "float_voltage"
CONF_TEMP_COMPENSATION = "temp_compensation"
CONF_BATTERIES_IN_SERIES = "batteries_in_series"
CONF_LOW_WARNING_VOLTAGE = "low_warning_voltage"
CONF_BATTERY_TYPE = "battery_type"
CONF_REMOTE_BATT_VOLTAGE_DETECT = "remote_batt_voltage_detect"
CONF_REMOTE_TEMP_DETECT = "remote_temp_detect"
CONF_BATTERY_RATED_VOLTAGE = "battery_rated_voltage"
CONF_LOW_SHUTDOWN_DETECT = "low_shutdown_detect"
CONF_WARNING_STATUS_BITS = "warning_status_bits"


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
        device_class=DEVICE_CLASS_CURRENT,
    ),
        cv.Optional(CONF_CHARGING_CURRENT1): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        icon="mdi:current-dc",
        device_class=DEVICE_CLASS_CURRENT,
    ),
    cv.Optional(CONF_CHARGING_CURRENT2): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        icon="mdi:current-dc",
        device_class=DEVICE_CLASS_CURRENT,
    ),
    cv.Optional(CONF_CHARGING_POWER): sensor.sensor_schema(
        unit_of_measurement="W",
        accuracy_decimals=0,
        icon="mdi:flash",
        device_class=DEVICE_CLASS_POWER,
    ),
    cv.Optional(CONF_UNIT_TEMPERATURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        icon="mdi:thermometer",
        device_class=DEVICE_CLASS_TEMPERATURE,
    ),
    cv.Optional(CONF_REMOTE_BATTERY_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=2,
        icon="mdi:car-battery",
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    cv.Optional(CONF_REMOTE_BATTERY_TEMPERATURE): sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        icon="mdi:thermometer",
        device_class=DEVICE_CLASS_TEMPERATURE,
    ),
    cv.Optional(CONF_MAX_OUTPUT_POWER): sensor.sensor_schema(
        unit_of_measurement="W",
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        icon="mdi:flash"
    ),
    cv.Optional(CONF_NOMINAL_BATTERY_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    cv.Optional(CONF_NOMINAL_CHARGING_CURRENT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        icon="mdi:current-dc"
    ),
    cv.Optional(CONF_NOMINAL_CHARGING_CURRENT): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    cv.Optional(CONF_FLOAT_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_VOLTAGE,
    ),
    cv.Optional(CONF_TEMP_COMPENSATION): sensor.sensor_schema(
        unit_of_measurement="mV/°C",
        accuracy_decimals=1,
        icon="mdi:thermometer-lines"
    ),
    cv.Optional(CONF_BATTERIES_IN_SERIES): sensor.sensor_schema(
        accuracy_decimals=0,
        icon="mdi:battery"
    ),
    cv.Optional(CONF_LOW_WARNING_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_VOLTAGE,
    ), 
    cv.Optional(CONF_BATTERY_TYPE): text_sensor.text_sensor_schema(icon="mdi:battery"),
    cv.Optional(CONF_REMOTE_BATT_VOLTAGE_DETECT): text_sensor.text_sensor_schema(icon="mdi:battery-bluetooth"),
    cv.Optional(CONF_REMOTE_TEMP_DETECT): text_sensor.text_sensor_schema(icon="mdi:thermometer"),
    cv.Optional(CONF_BATTERY_RATED_VOLTAGE): text_sensor.text_sensor_schema(icon="mdi:battery-settings"),
    cv.Optional(CONF_LOW_SHUTDOWN_DETECT): text_sensor.text_sensor_schema(icon="mdi:power-settings"),
    cv.Optional(CONF_WARNING_STATUS_BITS): cv.ensure_list(binary_sensor.binary_sensor_schema()),
}).extend(cv.polling_component_schema("10s")).extend(uart.UART_DEVICE_SCHEMA)

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
    if CONF_CHARGING_CURRENT1 in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_CURRENT1])
        cg.add(var.set_charging_current_1_sensor(sens))

    if CONF_CHARGING_CURRENT2 in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_CURRENT2])
        cg.add(var.set_charging_current_2_sensor(sens))

    if CONF_CHARGING_POWER in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_POWER])
        cg.add(var.set_charging_power_sensor(sens))

    if CONF_UNIT_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_UNIT_TEMPERATURE])
        cg.add(var.set_unit_temperature_sensor(sens))

    if CONF_REMOTE_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_REMOTE_BATTERY_VOLTAGE])
        cg.add(var.set_remote_battery_voltage_sensor(sens))

    if CONF_REMOTE_BATTERY_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_REMOTE_BATTERY_TEMPERATURE])
        cg.add(var.set_remote_battery_temperature_sensor(sens))

    if CONF_MAX_OUTPUT_POWER in config:
        sens = await sensor.new_sensor(config[CONF_MAX_OUTPUT_POWER])
        cg.add(var.set_max_output_power_sensor(sens))

    if CONF_NOMINAL_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_NOMINAL_BATTERY_VOLTAGE])
        cg.add(var.set_nominal_battery_voltage_sensor(sens))

    if CONF_NOMINAL_CHARGING_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_NOMINAL_CHARGING_CURRENT])
        cg.add(var.set_nominal_charging_current_sensor(sens))

    if CONF_ABSORPTION_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_ABSORPTION_VOLTAGE])
        cg.add(var.set_absorption_voltage_sensor(sens))

    if CONF_FLOAT_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_FLOAT_VOLTAGE])
        cg.add(var.set_float_voltage_sensor(sens))

    if CONF_TEMP_COMPENSATION in config:
        sens = await sensor.new_sensor(config[CONF_TEMP_COMPENSATION])
        cg.add(var.set_temp_compensation_sensor(sens))

    if CONF_BATTERIES_IN_SERIES in config:
        sens = await sensor.new_sensor(config[CONF_BATTERIES_IN_SERIES])
        cg.add(var.set_batteries_in_series_sensor(sens))

    if CONF_LOW_WARNING_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_LOW_WARNING_VOLTAGE])
        cg.add(var.set_low_warning_voltage_sensor(sens))
    if CONF_BATTERY_TYPE in config:
        sens = await text_sensor.new_text_sensor(config[CONF_BATTERY_TYPE])
        cg.add(var.set_battery_type_text_sensor(sens))

    if CONF_REMOTE_BATT_VOLTAGE_DETECT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_REMOTE_BATT_VOLTAGE_DETECT])
        cg.add(var.set_remote_batt_voltage_detect_text_sensor(sens))

    if CONF_REMOTE_TEMP_DETECT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_REMOTE_TEMP_DETECT])
        cg.add(var.set_remote_temp_detect_text_sensor(sens))

    if CONF_BATTERY_RATED_VOLTAGE in config:
        sens = await text_sensor.new_text_sensor(config[CONF_BATTERY_RATED_VOLTAGE])
        cg.add(var.set_battery_rated_voltage_text_sensor(sens))

    if CONF_LOW_SHUTDOWN_DETECT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_LOW_SHUTDOWN_DETECT])
        cg.add(var.set_low_shutdown_detect_text_sensor(sens))
    if CONF_WARNING_STATUS_BITS in config:
        for i, conf in enumerate(config[CONF_WARNING_STATUS_BITS]):
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(var.set_warning_status_bit_sensor(i, sens))    


