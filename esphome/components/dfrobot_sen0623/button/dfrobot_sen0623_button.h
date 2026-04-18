#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

#include "../dfrobot_sen0623.h"

namespace esphome {
namespace dfrobot_sen0623 {

class DfrobotSen0623Button : public button::Button, public Component, public Parented<DfrobotSen0623Component> {};

class Sen0623ResetButton : public DfrobotSen0623Button {
 public:
  void press_action() override;
};

class Sen0623FallModeButton : public DfrobotSen0623Button {
 public:
  void press_action() override;
};

class Sen0623SleepModeButton : public DfrobotSen0623Button {
 public:
  void press_action() override;
};

}  // namespace dfrobot_sen0623
}  // namespace esphome
