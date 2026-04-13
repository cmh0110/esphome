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
                    processFrame(buffer, buffer_index);
                    buffer_index = 0;
                }
            }
        }

        void DfrobotSen0623Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "DfrobotSen0623Component");
        }

        void DfrobotSen0623Component::processFrame(uint8_t *buffer, int length) {
            // Minimum frame length check
            if (length < 10) return;
            
            // Check frame header (0x53, 0x59)
            if (buffer[0] != 0x53 || buffer[1] != 0x59) return;
            
            // Process different frame types based on the command bytes
            
                    if (operation == OP_REQ_HUMAN_DISTANCE) {
                        if (this->human_distance_sensor_ != nullptr) {
                            this->human_distance_sensor_->publish_state(data[0] << 8 | data[1]);
                        }
                    } else 
                    if (operation == OP_REQ_HUMAN_MOVE_RANGE) {
                        if (this->human_move_range_sensor_ != nullptr) {
                            this->human_move_range_sensor_->publish_state(data[0]);
                        }
                    }
            
            // Human presence information (0x80, 0x01)
            if (buffer[2] == 0x80 && buffer[3] == 0x01 && buffer[4] == 0x00 && buffer[5] == 0x01) {
                bool presence = buffer[6] > 0;
                if (this->presence_sensor_ != nullptr) {
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
                }
                ESP_LOGD("C1001", "Human presence: %s", presence ? "detected" : "not detected");
            }
            
            // Movement information (0x80, 0x02)
            else if (buffer[2] == 0x80 && buffer[3] == 0x02 && buffer[4] == 0x00 && buffer[5] == 0x01) {
                // 0x01 = stationary, 0x02 = active
                uint8_t movement = buffer[6];
                if (this->movement_text_sensor_ != nullptr) {
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
                }
                ESP_LOGD("C1001", "Movement status: %s", movement ? "active" : "still");
            }
            
            // Sleep state information (0x81, 0x01)
            else if (buffer[2] == 0x81 && buffer[3] == 0x01 && buffer[4] == 0x00 && buffer[5] == 0x01) {
            uint8_t state = buffer[6];
            std::string state_str;
            
            switch (state) {
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
            
            sleep_state_sensor->publish_state(state_str);
            ESP_LOGD("C1001", "Sleep state: %s", state_str.c_str());
            }
            
            // Respiratory rate value (0x81, 0x02)
            else if (buffer[2] == 0x81 && buffer[3] == 0x02 && buffer[4] == 0x00 && buffer[5] == 0x01) {
                uint8_t rate = buffer[6];
                if (this->breath_rate_sensor_ != nullptr) {
                    this->breath_rate_sensor_->publish_state(rate);
                }
                ESP_LOGD("C1001", "Respiration rate: %d bpm", rate);
            }
            
            // Heart rate value (0x85, 0x02)
            else if (buffer[2] == 0x85 && buffer[3] == 0x02 && buffer[4] == 0x00 && buffer[5] == 0x01) {
                uint8_t rate = buffer[6];
                
                if (this->heart_rate_sensor_ != nullptr) {
                    this->heart_rate_sensor_->publish_state(rate);
                }
                ESP_LOGD("C1001", "Heart rate: %d bpm", rate);
            }
        }

    } // namespace dfrobot_sen0623
} // namespace esphome
