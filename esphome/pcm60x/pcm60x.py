import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import uart, sensor
from esphome.const import CONF_ID, UNIT_VOLT, ICON_FLASH, DEVICE_CLASS_VOLTAGE

CODEOWNERS = ["@janisbeikm"]
DEPENDENCIES = ["uart"]

pcm60x_ns = cg.esphome_ns.namespace("pcm60x")
PCM60XComponent = pcm60x_ns.class_("PCM60XComponent", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(PCM60XComponent),
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
