import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_STATUS
)
from . import CONF_DFROBOT_SEN0623_ID, DfrobotSen0623Component

DEPENDENCIES = ["dfrobot_sen0623"]

CONF_HUMAN_MOVEMENT = "movement"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_DFROBOT_SEN0623_ID): cv.use_id(DfrobotSen0623Component),
            cv.Optional(CONF_STATUS): text_sensor.text_sensor_schema(),
            cv.Optional(CONF_HUMAN_MOVEMENT): text_sensor.text_sensor_schema(),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0623_ID])

    if status := config.get(CONF_STATUS):
        sens = await text_sensor.new_text_sensor(status)
        cg.add(parent.set_status_text_sensor(sens))
    if movement := config.get(CONF_HUMAN_MOVEMENT):
        sens = await text_sensor.new_text_sensor(movement)
        cg.add(parent.set_movement_text_sensor(sens))