import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_TYPE, ENTITY_CATEGORY_CONFIG
from esphome.cpp_generator import MockObjClass

from .. import CONF_DFROBOT_SEN0623_ID, DfrobotSen0623Component

DEPENDENCIES = ["dfrobot_sen0623"]

dfrobot_sen0623_ns = cg.esphome_ns.namespace("dfrobot_sen0623")
DfrobotSen0623Switch = dfrobot_sen0623_ns.class_(
    "DfrobotSen0623Switch",
    switch.Switch,
    cg.Component,
    cg.Parented.template(DfrobotSen0623Component),
)

Sen0623RequestRateSwitch = dfrobot_sen0623_ns.class_(
    "Sen0623RequestRateSwitch", DfrobotSen0623Switch
)
Sen0623HPLedSwitch = dfrobot_sen0623_ns.class_(
    "Sen0623HPLedSwitch", DfrobotSen0623Switch
)


def _switch_schema(class_: MockObjClass) -> cv.Schema:
    return (
        switch.switch_schema(
            class_,
            entity_category=ENTITY_CATEGORY_CONFIG,
        )
        .extend(
            {
                cv.GenerateID(CONF_DFROBOT_SEN0623_ID): cv.use_id(
                    DfrobotSen0623Component
                ),
            }
        )
        .extend(cv.COMPONENT_SCHEMA)
    )


CONFIG_SCHEMA = cv.typed_schema(
    {
        "request_rate": _switch_schema(Sen0623RequestRateSwitch),
        "hp_led": _switch_schema(Sen0623HPLedSwitch),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0623_ID])
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_switch")(var))
