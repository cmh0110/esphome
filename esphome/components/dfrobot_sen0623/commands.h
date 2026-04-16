#pragma once

#include <cstdint>
#include <string>

#include "esphome/core/helpers.h"

namespace esphome {
namespace dfrobot_sen0623 {

class DfrobotSen0623Component;

// Use command queue and time stamps to avoid blocking.
// When component has run time, check if minimum time (1s) between
// commands has passed. After that run a command from the queue.
class Command {
 public:
  virtual ~Command() = default;
  virtual uint8_t execute(DfrobotSen0623Component *parent);
  virtual uint8_t on_message(std::string &message) = 0;

 protected:
  DfrobotSen0623Component *parent_{nullptr};
  std::string cmd_;
  bool cmd_sent_{false};
  int8_t retries_left_{2};
  uint32_t cmd_duration_ms_{1000};
  uint32_t timeout_ms_{1500};
};

class ReadStateCommand : public Command {
 public:
  uint8_t execute(DfrobotSen0623Component *parent) override;
  uint8_t on_message(std::string &message) override;

 protected:
  uint32_t timeout_ms_{500};
};

}  // namespace dfrobot_sen0395
}  // namespace esphome
