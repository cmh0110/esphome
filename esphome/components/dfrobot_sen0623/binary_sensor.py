import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_PRESENCE,
)
from . import CONF_DFROBOT_SEN0623_ID, DfrobotSen0623Component

DEPENDENCIES = ["dfrobot_sen0623"]

CONF_PRESENCE = "presence"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_DFROBOT_SEN0623_ID): cv.use_id(DfrobotSen0623Component),
            cv.Optional(CONF_PRESENCE): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PRESENCE
            ),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0623_ID])

    if presence := config.get(CONF_PRESENCE):
        sens = await binary_sensor.new_binary_sensor(presence)
        cg.add(parent.set_presence_binary_sensor(sens))