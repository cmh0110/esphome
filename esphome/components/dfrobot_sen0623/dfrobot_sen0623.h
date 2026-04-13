#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/dfrobot_sen0623/DFRobot_HumanDetection.h"

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

class DfrobotSen0623Component : public uart::UARTDevice, public Component {
#ifdef USE_SWITCH
  SUB_SWITCH(request_rate)
  SUB_SWITCH(hp_led)
#endif


  public:
    bool process_packet(uint8_t *packetData, size_t len);

    void print_data(std::string tag, const uint8_t *bytes, size_t len);
    void configWorkMode(uint8_t mode);
    void sensorReset();
    void populateData();

    // sensor
    void set_heart_rate_sensor(sensor::Sensor *rate_sensor) { heart_rate_sensor_ = rate_sensor; }
    void set_breath_rate_sensor(sensor::Sensor *rate_sensor) { breath_rate_sensor_ = rate_sensor; }
    void set_human_distance_sensor(sensor::Sensor *human_distance_sensor) { human_distance_sensor_ = human_distance_sensor; }
    void set_human_move_range_sensor(sensor::Sensor *human_move_range_sensor) { human_move_range_sensor_ = human_move_range_sensor; }
    // text sensor 
    void set_status_text_sensor(text_sensor::TextSensor *status_text_sensor) { status_text_sensor_ = status_text_sensor; }
    void set_movement_text_sensor(text_sensor::TextSensor *movement_text_sensor) { movement_text_sensor_ = movement_text_sensor; }
    // binary_sensor
    void set_presence_binary_sensor(binary_sensor::BinarySensor *presence_sensor) { presence_sensor_ = presence_sensor; }
    // button
    void set_reset_button(button::Button *reset_button) { reset_button_ = reset_button; }
    void set_mode_fall_button(button::Button *mode_fall_button) { mode_fall_button_ = mode_fall_button; }
    void set_mode_sleep_button(button::Button *mode_sleep_button) { mode_sleep_button_ = mode_sleep_button; }
    // switch
    void set_switch_request_rate(bool val);
    void set_switch_hp_led(bool val);
    // actions
    void cmd_reset();
    void cmd_mode_fall();
    void cmd_mode_sleep();

    void setup() override;
    void loop() override;
    void dump_config() override;
  protected:
    sensor::Sensor *heart_rate_sensor_{nullptr};
    sensor::Sensor *breath_rate_sensor_{nullptr};
    sensor::Sensor *human_distance_sensor_{nullptr};
    sensor::Sensor *human_move_range_sensor_{nullptr};
    binary_sensor::BinarySensor *presence_sensor_{nullptr};

    text_sensor::TextSensor *status_text_sensor_{nullptr};
    text_sensor::TextSensor *movement_text_sensor_{nullptr};
    text_sensor::TextSensor *sleep_state_text_sensor_{nullptr};

    button::Button *reset_button_{nullptr};
    button::Button *mode_fall_button_{nullptr};
    button::Button *mode_sleep_button_{nullptr};
    DFRobot_HumanDetection::DFRobot_HumanDetection sen0623_;
};


}  // namespace dfrobot_sen0623
}  // namespace esphome