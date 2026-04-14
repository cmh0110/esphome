import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import (
    CONF_ID,
)
DEPENDENCIES = ["uart"]

dfrobot_sen0623_ns = cg.esphome_ns.namespace("dfrobot_sen0623")
DfrobotSen0623Component = dfrobot_sen0623_ns.class_(
    "DfrobotSen0623Component", cg.Component
)

CONF_DFROBOT_SEN0623_ID = "dfrobot_sen0623_id"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DfrobotSen0623Component),
        }
    ).extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
