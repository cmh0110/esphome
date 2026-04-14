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
std::pair<uint8_t, uint8_t> OP_SLEEP_STATE = {0x84, 0x82};
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
            sen0623_.configWorkMode(DFRobot_HumanDetection::eWorkMode::eFallingMode);
        }

        void DfrobotSen0623Component::cmd_mode_sleep()
        {
            sen0623_.configWorkMode(DFRobot_HumanDetection::eWorkMode::eSleepMode);
        }

        void DfrobotSen0623Component::setup()
        {
            // ESP_LOGD("Sending sleep command", "Set mode to sleep for DfrobotSen0623Component...");
            //cmd_mode_sleep();
            // ESP_LOGD("Set sleep command", "Set mode to sleep for DfrobotSen0623Component...");
            // delay(100);
        }

        void DfrobotSen0623Component::loop()
        {
            static uint8_t buffer[64];
            static int buffer_index = 0;
            
            while (available()) {
            uint8_t byte = read();
            
            // Store byte in buffer
            if (buffer_index < sizeof(buffer)) {
                buffer[buffer_index++] = byte;
            }
            
            // Check for end of frame marker (0x54, 0x43)
            if (buffer_index >= 2 && buffer[buffer_index - 2] == 0x54 && buffer[buffer_index - 1] == 0x43) {
                ESP_LOGD("Try to pop data", "Calling populateData for DfrobotSen0623Component...");
                populateData(buffer, buffer_index);
                ESP_LOGD("Poped data", "Finished populateData for DfrobotSen0623Component...");
                buffer_index = 0;
            }
            }
        }

        void DfrobotSen0623Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "DfrobotSen0623Component");
        }

        void DfrobotSen0623Component::populateData(uint8_t *buffer, int length) {
            std::pair<uint8_t, uint8_t> operation = {buffer[2], buffer[3]};
            if(operation == OP_REQ_HUMAN_DISTANCE) {
                ESP_LOGD(TAG, "Received human distance data");
                if (this->human_distance_sensor_ != nullptr) {
                    this->human_distance_sensor_->publish_state(buffer[6] << 8 | buffer[7]);
                }
            } else if(operation == OP_REQ_HUMAN_MOVE_RANGE) {
                ESP_LOGD(TAG, "Received human move range data");
                if (this->human_move_range_sensor_ != nullptr) {
                    this->human_move_range_sensor_->publish_state(buffer[6]);
                }
            } else if(operation == OP_REQ_HUMAN_PRESENCE) {
                ESP_LOGD(TAG, "Received human presence data");
                if (this->presence_sensor_ != nullptr) {
                    uint16_t presence = buffer[6];
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
            } else if(operation == OP_REQ_HUMAN_MOVEMENT) {
                ESP_LOGD(TAG, "Received human movement data");
                if (this->movement_text_sensor_ != nullptr) {
                    uint16_t movement = buffer[6];
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
            } else if(operation == OP_SLEEP_STATE) {
                ESP_LOGD(TAG, "Received sleep state data");
                if (this->sleep_state_text_sensor_ != nullptr) {
                    uint8_t sleep_state = buffer[6];
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
            } else if(operation == OP_REQ_BREATH_RATE) {
                ESP_LOGD(TAG, "Received breath rate data");
                if (this->breath_rate_sensor_ != nullptr) {
                    uint8_t rate = buffer[6];
                    this->breath_rate_sensor_->publish_state(rate);
                    ESP_LOGD("C1001", "Respiration rate: %d bpm", rate);
                }
            } else if(operation == OP_REQ_HEART_RATE) {
                ESP_LOGD(TAG, "Received heart rate data");
                if (this->heart_rate_sensor_ != nullptr) {
                    uint8_t rate = buffer[6];
                    this->heart_rate_sensor_->publish_state(rate);
                    ESP_LOGD("C1001", "Heart rate: %d bpm", rate);
                }
            } else {
                ESP_LOGD(TAG, "Received unknown operation: %02X %02X", operation.first, operation.second);
            }
        }

    } // namespace dfrobot_sen0623
} // namespace esphome
