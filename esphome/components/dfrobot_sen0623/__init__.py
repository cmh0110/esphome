import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    CONF_MOTION,
    DEVICE_CLASS_MOTION
)
DEPENDENCIES = ["uart"]

CONF_DFROBOT_SEN0623_ID = "dfrobot_sen0623_id"

dfrobot_sen0623_ns = cg.esphome_ns.namespace("dfrobot_sen0623")
DfrobotSen0623Component = dfrobot_sen0623_ns.class_(
    "DfrobotSen0623Component", uart.UARTDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DfrobotSen0623Component),
            cv.Optional(CONF_MOTION): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_MOTION
            ),
            
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
