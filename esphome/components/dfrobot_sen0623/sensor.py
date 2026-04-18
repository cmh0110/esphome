import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_BEATS_PER_MINUTE,
    UNIT_CENTIMETER,
)
from . import CONF_DFROBOT_SEN0623_ID, DfrobotSen0623Component

DEPENDENCIES = ["dfrobot_sen0623"]

CONF_HEART_RATE = "heart_rate"
CONF_BREATH_RATE = "breath_rate"

CONF_HUMAN_DISTANCE = "human_distance"
CONF_HUMAN_MOVE_RANGE = "human_move_range"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_DFROBOT_SEN0623_ID): cv.use_id(DfrobotSen0623Component),
            cv.Optional(CONF_HEART_RATE): sensor.sensor_schema(
                unit_of_measurement=UNIT_BEATS_PER_MINUTE,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BREATH_RATE): sensor.sensor_schema(
                unit_of_measurement=UNIT_BEATS_PER_MINUTE,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMAN_DISTANCE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CENTIMETER,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMAN_MOVE_RANGE): sensor.sensor_schema(
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0623_ID])

    if heart_rate := config.get(CONF_HEART_RATE):
        sens = await sensor.new_sensor(heart_rate)
        cg.add(parent.set_heart_rate_sensor(sens))

    if breath_rate := config.get(CONF_BREATH_RATE):
        sens = await sensor.new_sensor(breath_rate)
        cg.add(parent.set_breath_rate_sensor(sens))

    if human_distance := config.get(CONF_HUMAN_DISTANCE):
        sens = await sensor.new_sensor(human_distance)
        cg.add(parent.set_human_distance_sensor(sens))

    if human_move_range := config.get(CONF_HUMAN_MOVE_RANGE):
        sens = await sensor.new_sensor(human_move_range)
        cg.add(parent.set_human_move_range_sensor(sens))