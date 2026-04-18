#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include "../dfrobot_sen0623.h"

namespace esphome {
namespace dfrobot_sen0623 {

class DfrobotSen0623Switch : public switch_::Switch, public Component, public Parented<DfrobotSen0623Component> {};

class Sen0623RequestRateSwitch : public DfrobotSen0623Switch {
 public:
  void write_state(bool state) override;
};

class Sen0623HPLedSwitch : public DfrobotSen0623Switch {
 public:
  void write_state(bool state) override;
};

}  // namespace dfrobot_sen0623
}  // namespace esphome
