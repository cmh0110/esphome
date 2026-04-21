#pragma once

#include "DFRobot_HumanDetection.h"

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "commands.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

namespace esphome {
namespace dfrobot_sen0623 {

static const uint8_t COMMAND_QUEUE_SIZE = 20;

class DfrobotSen0623Component;

class CircularCommandQueue {
 public:
  int8_t enqueue(std::unique_ptr<Command> cmd);
  std::unique_ptr<Command> dequeue();
  bool is_empty();
  bool is_full();
  uint8_t process(DfrobotSen0623Component *parent);

 protected:
  int front_{-1};
  int rear_{-1};
  std::unique_ptr<Command> commands_[COMMAND_QUEUE_SIZE];
};

class DfrobotSen0623Component : public uart::UARTDevice, public Component {
// #ifdef USE_SWITCH
//   SUB_SWITCH(request_rate)
//   SUB_SWITCH(hp_led)
// #endif


  public:

    int8_t populateData();
    int totalCommands{6};
    int currentCommand{0};

    // sensor
    void set_heart_rate_sensor(sensor::Sensor *rate_sensor) { heart_rate_sensor_ = rate_sensor; }
    void set_breath_rate_sensor(sensor::Sensor *rate_sensor) { breath_rate_sensor_ = rate_sensor; }
    void set_human_distance_sensor(sensor::Sensor *human_distance_sensor) { human_distance_sensor_ = human_distance_sensor; }
    void set_human_move_range_sensor(sensor::Sensor *human_move_range_sensor) { human_move_range_sensor_ = human_move_range_sensor; }
    // text sensor 
    void set_status_text_sensor(text_sensor::TextSensor *status_text_sensor) { status_text_sensor_ = status_text_sensor; }
    void set_movement_text_sensor(text_sensor::TextSensor *movement_text_sensor) { movement_text_sensor_ = movement_text_sensor; }
    void set_sleep_state_text_sensor(text_sensor::TextSensor *sleep_state_text_sensor) { sleep_state_text_sensor_ = sleep_state_text_sensor; }
    // binary_sensor
    void set_presence_binary_sensor(binary_sensor::BinarySensor *presence_sensor) { presence_sensor_ = presence_sensor; }
    // button
    void set_reset_button(button::Button *reset_button) { reset_button_ = reset_button; }
    void set_mode_fall_button(button::Button *mode_fall_button) { mode_fall_button_ = mode_fall_button; }
    void set_mode_sleep_button(button::Button *mode_sleep_button) { mode_sleep_button_ = mode_sleep_button; }
    // switch
    // void set_switch_request_rate(bool val);
    // void set_switch_hp_led(bool val);
    // actions
    void cmd_reset();
    void cmd_mode_fall();
    void cmd_mode_sleep();

    void setup() override;
    void loop() override;
    void dump_config() override;

    int8_t enqueue(std::unique_ptr<Command> cmd);
  protected:
    sensor::Sensor *heart_rate_sensor_{nullptr};
    sensor::Sensor *breath_rate_sensor_{nullptr};
    sensor::Sensor *human_distance_sensor_{nullptr};
    sensor::Sensor *human_move_range_sensor_{nullptr};

    text_sensor::TextSensor *status_text_sensor_{nullptr};
    text_sensor::TextSensor *movement_text_sensor_{nullptr};
    text_sensor::TextSensor *sleep_state_text_sensor_{nullptr};

    binary_sensor::BinarySensor *presence_sensor_{nullptr};

    button::Button *reset_button_{nullptr};
    button::Button *mode_fall_button_{nullptr};
    button::Button *mode_sleep_button_{nullptr};

    CircularCommandQueue cmd_queue_;
    uint32_t ts_last_cmd_sent_{0};

    uint8_t read_message_();
    uint8_t find_prompt_();
    uint8_t send_cmd_(const char *cmd, uint32_t duration);

    uint8_t messageAvailable_();
    void uartWrite_(uint8_t data);
    void uartReadByte_(uint8_t *data);

    friend class DFRobot_HumanDetection;
    friend class Command;
    friend class ReadStateCommand;

    DFRobot_HumanDetection sen0623_ = DFRobot_HumanDetection();
};


}  // namespace dfrobot_sen0623
}  // namespace esphome