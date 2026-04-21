#include "commands.h"

#include <cmath>

#include "esphome/core/log.h"

#include "dfrobot_sen0623.h"

namespace esphome {
namespace dfrobot_sen0623 {

static const char *const TAG = "dfrobot_sen0623.commands";

uint8_t Command::execute(DfrobotSen0623Component *parent) {
  this->parent_ = parent;
  if (this->cmd_sent_) {
    int8_t rc = this->parent_->populateData();
    if (millis() - this->parent_->ts_last_cmd_sent_ > this->timeout_ms_) {
      ESP_LOGD(TAG, "Command timeout");
      if (this->retries_left_ > 0) {
        this->retries_left_ -= 1;
        this->cmd_sent_ = false;
        ESP_LOGD(TAG, "Retrying");
      } else {
        return 1;  // Command done
      }
    }
    if (rc) {
      this->cmd_sent_ = true;
      return rc;
    }
  }
  return 0;  // Command not done yet
}

uint8_t ReadStateCommand::execute(DfrobotSen0623Component *parent) {
  this->parent_ = parent;
  if (this->parent_->populateData(currentCommand)) {
    currentCommand++;
    if (currentCommand > this->parent_->totalCommands) {
      currentCommand = 0;
    }
    return 1;  // Command done
  }
  if (millis() - this->parent_->ts_last_cmd_sent_ > this->timeout_ms_) {
    return 1;  // Command done, timeout
  }
  return 0;  // Command not done yet.
}

uint8_t ReadStateCommand::on_message(std::string &message) { return 1; }

}  // namespace dfrobot_sen0623
}  // namespace esphome
