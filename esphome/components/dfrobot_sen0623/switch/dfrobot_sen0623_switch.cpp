#include "dfrobot_sen0623_switch.h"

namespace esphome {
namespace dfrobot_sen0623 {

void Sen0623RequestRateSwitch::write_state(bool state) {
  this->parent_->set_switch_request_rate(state);
}

void Sen0623HPLedSwitch::write_state(bool state) {
  this->parent_->set_switch_hp_led(state);
}
}  // namespace dfrobot_sen0623
}  // namespace esphome
