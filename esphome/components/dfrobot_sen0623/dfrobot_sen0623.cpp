#include "dfrobot_sen0623.h"
#include "esphome/core/log.h"

#include "esphome/core/helpers.h"


std::pair<uint8_t, uint8_t> OP_RST_SENSOR = {0x01, 0x07};
std::pair<uint8_t, uint8_t> OP_INIT = {0x01, 0x83};
std::pair<uint8_t, uint8_t> OP_REQ_MODE = {0x02, 0xA8};
std::pair<uint8_t, uint8_t> OP_REQ_HEART_RATE = {0x85, 0x02};
std::pair<uint8_t, uint8_t> OP_REQ_BREATH_RATE = {0x81, 0x02};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_PRESENCE = {0x80, 0x01};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_MOVEMENT = {0x80, 0x02};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_MOVE_RANGE = {0x80, 0x03};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_DISTANCE = {0x80, 0x04};
std::pair<uint8_t, uint8_t> OP_SLEEP_STATE = {0x81, 0x01};
std::pair<uint8_t, uint8_t> OP_SET_MODE = {0x01, 0x01};
uint8_t MODE_SLEEP = 0x02;
uint8_t MODE_FALL = 0x01;
int currentCommand = 0;

namespace esphome
{
    namespace dfrobot_sen0623
    {

        static const char *TAG = "dfrobot_sen0623.component";

        void DfrobotSen0623Component::cmd_reset()
        {
            // uint8_t data = 0x0f;
            // uint8_t buf[10];
            // uint8_t cmdBuf[10] = {0x53, 0x59, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x54, 0x43};
            // write_array(cmdBuf, 10);
            sen0623_.sensorRet();
        }

        void DfrobotSen0623Component::cmd_mode_fall()
        {
            // uint8_t data = 0x0f;
            // uint8_t buf[10];
            // uint8_t cmdBuf[10] = {0x53, 0x59, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x54, 0x43};
            // cmdBuf[6] = MODE_FALL;
            // cmdBuf[7] = sumData(7, cmdBuf);
            // write_array(cmdBuf, 10);
            sen0623_.configWorkMode(DFRobot_HumanDetection::eFallingMode);
            cmd_reset();
        }

        void DfrobotSen0623Component::cmd_mode_sleep()
        {
            // uint8_t data = 0x0f;
            // uint8_t buf[10];
            // uint8_t cmdBuf[10] = {0x53, 0x59, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x54, 0x43};
            // cmdBuf[6] = MODE_SLEEP;
            // cmdBuf[7] = sumData(7, cmdBuf);
            // write_array(cmdBuf, 10);
            sen0623_.configWorkMode(DFRobot_HumanDetection::eSleepMode);
            cmd_reset();
        }

        void DfrobotSen0623Component::setup()
        {
            // ESP_LOGD("Sending sleep command", "Set mode to sleep for DfrobotSen0623Component...");
            sen0623_.parent_ = this;
            cmd_mode_sleep();
            // ESP_LOGD("Set sleep command", "Set mode to sleep for DfrobotSen0623Component...");
            // delay(100);
        }

        void DfrobotSen0623Component::loop()
        {
            if (cmd_queue_.is_empty()) {
                // Command queue empty. Read sensor state.
                cmd_queue_.enqueue(make_unique<ReadStateCommand>());//causing problems
            }

            // Commands are non-blocking and need to be called repeatedly.
            if (cmd_queue_.process(this)) {
                // Dequeue if command is done
                cmd_queue_.dequeue();
            }
        }

        int8_t DfrobotSen0623Component::enqueue(std::unique_ptr<Command> cmd) {
            return cmd_queue_.enqueue(std::move(cmd));  // Transfer ownership using std::move
        }

        void DfrobotSen0623Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "DfrobotSen0623Component");
        }

        int8_t DfrobotSen0623Component::populateData() {
            // std::pair<uint8_t, uint8_t> operation = {buffer[2], buffer[3]};
            // if(operation == OP_REQ_HUMAN_DISTANCE) {
            //     ESP_LOGD(TAG, "Received human distance data");
            // if (millis() - ts_last_cmd_sent_ > 200) {
            //     ts_last_cmd_sent_ = millis();
                if ((this->human_distance_sensor_ != nullptr) && currentCommand == 0) {
                    // this->human_distance_sensor_->publish_state(buffer[6] << 8 | buffer[7]);
                    uint16_t distance = sen0623_.smHumanData(DFRobot_HumanDetection::eHumanDistance);
                    this->human_distance_sensor_->publish_state(distance);
                    ESP_LOGD("C1001", "Human distance: %d cm", distance);
                }
            // } else if(operation == OP_REQ_HUMAN_MOVE_RANGE) {
            //     ESP_LOGD(TAG, "Received human move range data");
                if ((this->human_move_range_sensor_ != nullptr) && currentCommand == 1) {
                    // this->human_move_range_sensor_->publish_state(buffer[6]);
                    uint16_t move_range = sen0623_.smHumanData(DFRobot_HumanDetection::eHumanMovingRange);
                    this->human_move_range_sensor_->publish_state(move_range);
                    ESP_LOGD("C1001", "Human move range: %d cm", move_range);
                }
            // } else if(operation == OP_REQ_HUMAN_PRESENCE) {
            //     ESP_LOGD(TAG, "Received human presence data");
                if ((this->presence_sensor_ != nullptr) && currentCommand == 2) {
                    // uint16_t presence = buffer[6];
                    uint16_t presence = sen0623_.smHumanData(DFRobot_HumanDetection::eHumanPresence);
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
            // } else if(operation == OP_REQ_HUMAN_MOVEMENT) {
            //     ESP_LOGD(TAG, "Received human movement data");
                if ((this->movement_text_sensor_ != nullptr) && currentCommand == 3) {
                    // uint16_t movement = buffer[6];
                    uint16_t movement = sen0623_.smHumanData(DFRobot_HumanDetection::eHumanMovement);
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
            // } else if(operation == OP_SLEEP_STATE) {
            //     ESP_LOGD(TAG, "Received sleep state data");
                if ((this->sleep_state_text_sensor_ != nullptr) && currentCommand == 4) {
                    // uint8_t sleep_state = buffer[6];
                    uint8_t sleep_state = sen0623_.smSleepData(DFRobot_HumanDetection::eSleepState);
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
            // } else if(operation == OP_REQ_BREATH_RATE) {
            //     ESP_LOGD(TAG, "Received breath rate data");
                if ((this->breath_rate_sensor_ != nullptr) && currentCommand == 5) {
                    // uint8_t rate = buffer[6];
                    uint8_t rate = sen0623_.getBreatheValue();
                    this->breath_rate_sensor_->publish_state(rate);
                    ESP_LOGD("C1001", "Respiration rate: %d bpm", rate);
                }
            // } else if(operation == OP_REQ_HEART_RATE) {
            //     ESP_LOGD(TAG, "Received heart rate data");
                if ((this->heart_rate_sensor_ != nullptr) && currentCommand == 6) {
                    // uint8_t rate = buffer[6];
                    uint8_t rate = sen0623_.getHeartRate();
                    this->heart_rate_sensor_->publish_state(rate);
                    ESP_LOGD("C1001", "Heart rate: %d bpm", rate);
                }
                currentCommand++;
                return 1;
            // }
            // return 0;
        }

    // uint8_t DfrobotSen0623Component::sumData(uint8_t len, uint8_t *buf)
    // {
    //     uint16_t data = 0;
    //     uint8_t *_buf = buf;
    //     for (uint8_t i = 0; i < len; i++)
    //     {
    //         data += _buf[i];
    //     }
    //     return data & 0xff;
    // }

    uint8_t DfrobotSen0623Component::messageAvailable_() {
        return this->available();
    }

    void DfrobotSen0623Component::uartWrite_(uint8_t data) {
        this->write_byte(data);
    }

    void DfrobotSen0623Component::uartReadByte_(uint8_t *data) {
        this->read_byte(data);
    }

    int8_t CircularCommandQueue::enqueue(std::unique_ptr<Command> cmd) {
    if (this->is_full()) {
        ESP_LOGE(TAG, "Command queue is full");
        return -1;
    } else if (this->is_empty())
        front_++;
    rear_ = (rear_ + 1) % COMMAND_QUEUE_SIZE;
    commands_[rear_] = std::move(cmd);  // Transfer ownership using std::move
    return 1;
    }

    std::unique_ptr<Command> CircularCommandQueue::dequeue() {
    if (this->is_empty())
        return nullptr;
    std::unique_ptr<Command> dequeued_cmd = std::move(commands_[front_]);
    if (front_ == rear_) {
        front_ = -1;
        rear_ = -1;
    } else {
        front_ = (front_ + 1) % COMMAND_QUEUE_SIZE;
    }

    return dequeued_cmd;
    }

    bool CircularCommandQueue::is_empty() { return front_ == -1; }

    bool CircularCommandQueue::is_full() { return (rear_ + 1) % COMMAND_QUEUE_SIZE == front_; }

    // Run execute method of first in line command.
    // Execute is non-blocking and has to be called until it returns 1.
    uint8_t CircularCommandQueue::process(DfrobotSen0623Component *parent) {
    if (!is_empty()) {
        return commands_[front_]->execute(parent);
    } else {
        return 1;
    }
    }

} // namespace dfrobot_sen0623
} // namespace esphome
