#include "dfrobot_sen0623_button.h"

namespace esphome {
namespace dfrobot_sen0623 {

void Sen0623ResetButton::press_action() {
   this->parent_->cmd_reset();
}

void Sen0623FallModeButton::press_action() {
   this->parent_->cmd_mode_fall();
}

void Sen0623SleepModeButton::press_action() {
   this->parent_->cmd_mode_sleep();
}

}  // namespace dfrobot_sen0623
}  // namespace esphome
