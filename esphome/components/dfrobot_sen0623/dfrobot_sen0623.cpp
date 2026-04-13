#include "dfrobot_sen0623.h"
#include "esphome/core/log.h"

#include "esphome/core/helpers.h"


std::pair<uint8_t, uint8_t> OP_RST_SENSOR = {0x01, 0x07};
std::pair<uint8_t, uint8_t> OP_INIT = {0x01, 0x83};
std::pair<uint8_t, uint8_t> OP_REQ_MODE = {0x02, 0xA8};
std::pair<uint8_t, uint8_t> OP_REQ_HEART_RATE = {0x85, 0x82};
std::pair<uint8_t, uint8_t> OP_REQ_BREATH_RATE = {0x81, 0x82};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_PRESENCE = {0x80, 0x81};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_MOVEMENT = {0x80, 0x82};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_MOVE_RANGE = {0x80, 0x83};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_DISTANCE = {0x80, 0x84};
std::pair<uint8_t, uint8_t> OP_SET_MODE = {0x01, 0x01};
uint8_t MODE_SLEEP = 0x02;
uint8_t MODE_FALL = 0x01;

namespace esphome
{
    namespace dfrobot_sen0623
    {

        static const char *TAG = "dfrobot_sen0623.component";

        bool _switch_request_rate = false;

        void DfrobotSen0623Component::cmd_reset()
        {
            // uint8_t payload[1] = {0x0f};
            // this->forge_packet(0x01, 0x02, payload, sizeof(payload));
            sen0623_.sensorRet();
            //delay(2000);
            //this->request(4);
        }

        void DfrobotSen0623Component::cmd_mode_fall()
        {
            sen0623_.configWorkMode(DFRobot_HumanDetection::DFRobot_HumanDetection::eWorkMode::eFallingMode);
        }

        void DfrobotSen0623Component::cmd_mode_sleep()
        {
            sen0623_.configWorkMode(DFRobot_HumanDetection::DFRobot_HumanDetection::eWorkMode::eSleepingMode);
        }

        void DfrobotSen0623Component::setup()
        {
            sen0623_.begin();
            cmd_mode_sleep();
            delay(100);

        }

        void DfrobotSen0623Component::loop()
        {
            populateData();
            delay(50);
        }

        void DfrobotSen0623Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "DfrobotSen0623Component");
        }

        void DfrobotSen0623Component::populateData() {
            if (this->human_distance_sensor_ != nullptr) {
                this->human_distance_sensor_->publish_state(sen0623_.smHumanData(DFRobot_HumanDetection::DFRobot_HumanDetection::eHumanDistance));
            }
            if (this->human_move_range_sensor_ != nullptr) {
                this->human_move_range_sensor_->publish_state(sen0623_.smHumanData(DFRobot_HumanDetection::DFRobot_HumanDetection::eHumanMovingRange));
            }
            if (this->presence_sensor_ != nullptr) {
                uint8_t presence = sen0623_.smHumanData(DFRobot_HumanDetection::DFRobot_HumanDetection::eHumanPresence);
                switch (presence)
                {
                case 0:
                    this->presence_sensor_->publish_state(0);
                    break;
                case 1:
                    this->presence_sensor_->publish_state(1);
                    break;
                default:
                    ESP_LOGE(TAG, "INVALID PRESENCE: %02X", presence);
                    break;
                }
                ESP_LOGD("C1001", "Human presence: %s", presence ? "detected" : "not detected");
            }
            if (this->movement_text_sensor_ != nullptr) {
                uint8_t movement = sen0623_.smHumanData(DFRobot_HumanDetection::DFRobot_HumanDetection::eHumanMovement);
                switch (movement)
                {
                case 0:
                    this->movement_text_sensor_->publish_state("none");
                    break;
                case 1:
                    this->movement_text_sensor_->publish_state("still");
                    break;
                case 2:
                    this->movement_text_sensor_->publish_state("active");
                    break;
                default:
                    ESP_LOGD(TAG, "INVALID MOVEMENT: %02X", movement);
                    break;
                }
                ESP_LOGD("C1001", "Movement status: %s", movement ? "active" : "still");
            }
            if (this->sleep_state_text_sensor_ != nullptr) {
                uint8_t sleep_state = sen0623_.smHumanData(DFRobot_HumanDetection::DFRobot_HumanDetection::eHumanMovement);
                std::string state_str;
                
                switch (sleep_state) {
                    case 0:
                    state_str = "Deep Sleep";
                    break;
                    case 1:
                    state_str = "Light Sleep";
                    break;
                    case 2:
                    state_str = "Awake";
                    break;
                    case 3:
                    state_str = "None";
                    break;
                    default:
                    state_str = "Unknown";
                }
                this->sleep_state_text_sensor_->publish_state(state_str);
                ESP_LOGD("C1001", "Sleep state: %s", state_str.c_str());
            }
            if (this->breath_rate_sensor_ != nullptr) {
                uint8_t rate = sen0623_.getBreatheValue();
                this->breath_rate_sensor_->publish_state(rate);
                ESP_LOGD("C1001", "Respiration rate: %d bpm", rate);
            }
            if (this->heart_rate_sensor_ != nullptr) {
                uint8_t rate = sen0623_.getHeartRate();
                this->heart_rate_sensor_->publish_state(rate);
                ESP_LOGD("C1001", "Heart rate: %d bpm", rate);
            }
        }

    } // namespace dfrobot_sen0623
} // namespace esphome
