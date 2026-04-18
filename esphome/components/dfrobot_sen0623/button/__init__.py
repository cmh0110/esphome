import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_TYPE, ENTITY_CATEGORY_CONFIG
from esphome.cpp_generator import MockObjClass

from .. import CONF_DFROBOT_SEN0623_ID, DfrobotSen0623Component

DEPENDENCIES = ["dfrobot_sen0623"]

dfrobot_sen0623_ns = cg.esphome_ns.namespace("dfrobot_sen0623")
DfrobotSen0623Button = dfrobot_sen0623_ns.class_(
    "DfrobotSen0623Button",
    button.Button,
    cg.Component,
    cg.Parented.template(DfrobotSen0623Component),
)

Sen0623ResetButton = dfrobot_sen0623_ns.class_(
    "Sen0623ResetButton", DfrobotSen0623Button
)

Sen0623FallModeButton = dfrobot_sen0623_ns.class_(
    "Sen0623FallModeButton", DfrobotSen0623Button
)

Sen0623SleepModeButton = dfrobot_sen0623_ns.class_(
    "Sen0623SleepModeButton", DfrobotSen0623Button
)


def _button_schema(class_: MockObjClass) -> cv.Schema:
    return (
        button.button_schema(
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
        "reset": _button_schema(Sen0623ResetButton),
        "mode_fall": _button_schema(Sen0623FallModeButton),
        "mode_sleep": _button_schema(Sen0623SleepModeButton),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DFROBOT_SEN0623_ID])
    var = await button.new_button(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_button")(var))
