#include "dfrobot_sen0623.h"
#include "esphome/core/log.h"

#include "esphome/core/helpers.h"


std::pair<uint8_t, uint8_t> OP_RST_SENSOR = {0x01, 0x02};
std::pair<uint8_t, uint8_t> OP_INIT = {0x01, 0x83};
std::pair<uint8_t, uint8_t> OP_REQ_MODE = {0x02, 0xA8};
std::pair<uint8_t, uint8_t> OP_REQ_HEART_RATE = {0x85, 0x02};
std::pair<uint8_t, uint8_t> OP_REQ_BREATH_RATE = {0x81, 0x02};
std::pair<uint8_t, uint8_t> OP_REQ_HUMAN_PRESENCE = {0x80, 0x01};
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
            this->request(OP_RST_SENSOR);
            //delay(2000);
            //this->request(4);
        }

        void DfrobotSen0623Component::cmd_mode_fall()
        {
            uint8_t data[1];
            data[0] = {MODE_FALL};
            this->forge_packet(OP_SET_MODE.first, OP_SET_MODE.second, data, 1);
            this->request(OP_REQ_MODE);
        }

        void DfrobotSen0623Component::cmd_mode_sleep()
        {
            uint8_t data[1];
            data[0] = {MODE_SLEEP};
            this->forge_packet(OP_SET_MODE.first, OP_SET_MODE.second, data, 1);
            this->request(OP_REQ_MODE);
        }

        void DfrobotSen0623Component::request(std::pair<uint8_t, uint8_t> operation)
        {
            uint8_t data[1];
            data[0] = {0x0f};
            this->forge_packet(operation.first, operation.second, data, 1);
            // If i request something, should i wait for response?

        }

        bool _d = true;
        void DfrobotSen0623Component::forge_packet(uint8_t control, uint8_t command, uint8_t *senData, uint16_t senLen)
        {
            std::vector<uint8_t> buffer;
            buffer.push_back(0x53);
            buffer.push_back(0x59);
            buffer.push_back(control);
            buffer.push_back(command);
            buffer.push_back(0x00);
            buffer.push_back(0x01);
            for (uint8_t i = 0; i < senLen; i++)
            {
                buffer.push_back(senData[i]);
            }
            buffer.push_back(0x00);
            buffer.push_back(0x54);
            buffer.push_back(0x43);

            // this->print_data("XX", buffer.data(), buffer.size());
            this->send_packet(buffer.data(), buffer.size());
        }

        void DfrobotSen0623Component::send_packet(uint8_t *packetData, size_t len)
        {

            if (_d)
            {
                this->print_data(">>", packetData, len);
            }
            for (uint8_t i = 0; i < len; i++)
            {
                this->write_byte(packetData[i]);
            }
        }

        uint8_t DfrobotSen0623Component::read_packet(uint8_t *packetData)
        {
            if (!this->available()) {
                return 0;
            }
            std::vector<uint8_t> buffer;
            uint8_t byte;

            // Read bytes until '\n' delimiter or no more bytes available
            while (this->available() && this->read_byte(&byte))
            {
                buffer.push_back(byte);
                if (byte == '\n')
                {
                    break;
                }
            }
            // Copy data to packetData and return the length
            size_t len = buffer.size();
            if (len > 0)
            {
                // Make sure to not overflow packetData buffer — adjust max length accordingly
                // For example, if packetData is fixed size 100 bytes:
                size_t max_len = 100; // Change as needed
                if (len > max_len)
                {
                    len = max_len;
                }
                memcpy(packetData, buffer.data(), len);
            }

            if (_d && len > 0)
            {
                this->print_data("<<", packetData, len);
            }

            return (uint8_t)len;
        }

        uint8_t DfrobotSen0623Component::wait_for_packet(std::pair<uint8_t, uint8_t> operation) {
            // I think this can be a lot lower
            uint8_t ths = 128;
            while(ths > 0) {
                //ESP_LOGI(TAG, "%s", ths);
                uint8_t packetData[100]; // adjust size as needed
                uint8_t len = this->read_packet(packetData);

                if(this->process_packet(packetData, len)){
                    ths--;
                    if(packetData[2] == operation.first && packetData[3] == operation.second) {
                        return packetData[6];
                    } 
                }

            }
            ESP_LOGI(TAG, "WAIT FAILED");
            return 0xf5;
        }

        bool DfrobotSen0623Component::process_packet(uint8_t *packetData, size_t len)
        {
            // Process only valid packets
            if (len > 5)
            {
                uint8_t dataLen = ((uint16_t)packetData[4] << 8) | packetData[5];
                uint8_t csum = 0;
                for (uint8_t i = 0; i < 6 + dataLen; i++)
                {
                    csum += packetData[i];
                }
                csum = csum & 0xff;
                if (packetData[0] == 0x53 && packetData[1] == 0x59 && packetData[len - 2] == 0x54 && packetData[len - 1] == 0x43 && csum == packetData[len - 3])
                {
                    uint8_t data[dataLen];
                    for (uint8_t i = 0; i < dataLen; i++)
                    {
                        data[i] = packetData[6 + i];
                    }

                    std::pair<uint8_t, uint8_t> operation = {packetData[2], packetData[3]};

                    if (operation == OP_REQ_HEART_RATE) {
                        if (data[0] > 0 && this->heart_rate_sensor_ != nullptr) {
                            this->heart_rate_sensor_->publish_state(data[0]);
                        }
                    } else 
                    if (operation == OP_REQ_BREATH_RATE) {
                        if (data[0] > 0 && this->breath_rate_sensor_ != nullptr) {
                            this->breath_rate_sensor_->publish_state(data[0]);
                        }
                    } else 
                    if (operation == OP_REQ_HUMAN_PRESENCE) {
                        if (this->presence_sensor_ != nullptr) {
                            switch (data[0])
                            {
                            case 0:
                                this->presence_sensor_->publish_state(0);
                                break;
                            case 1:
                                this->presence_sensor_->publish_state(1);
                                break;
                            default:
                                ESP_LOGE(TAG, "INVALID PRESENCE: %02X", data[0]);
                                break;
                            }
                        }
                    } else 
                    if (operation == OP_REQ_HUMAN_MOVEMENT) {
                        if (this->movement_text_sensor_ != nullptr) {
                            switch (data[0])
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
                                ESP_LOGE(TAG, "INVALID MOVEMENT: %02X", data[0]);
                                break;
                            }
                        }
                    } else 
                    if (operation == OP_REQ_HUMAN_DISTANCE) {
                        if (this->human_distance_sensor_ != nullptr) {
                            this->human_distance_sensor_->publish_state(data[0] << 8 | data[1]);
                        }
                    } else 
                    if (operation == OP_REQ_HUMAN_MOVE_RANGE) {
                        if (this->human_move_range_sensor_ != nullptr) {
                            this->human_move_range_sensor_->publish_state(data[0]);
                        }
                    } else 
                    if (operation == OP_REQ_MODE) {
                        if (this->status_text_sensor_ != nullptr)
                        {
                            switch (data[0])
                            {
                            case 1:
                                this->status_text_sensor_->publish_state("fall");
                                break;
                            case 2:
                                this->status_text_sensor_->publish_state("sleep");
                                break;
                            default:
                                this->status_text_sensor_->publish_state("error");
                                break;
                            }
                        }
                    } else 
                    if (
                        false 
                        || (operation.first == 0x01 && operation.second == 0x01) // 1
                        || (operation.first == 0x07 && operation.second == 0x07)      // 1
                        || (operation.first == 0x80 && operation.second == 0x02)      // 1
                        || (operation.first == 0x80 && operation.second == 0x03)      // 1
                        || (operation.first == 0x80 && operation.second == 0x04)      // 2
                        || (operation.first == 0x80 && operation.second == 0x05)      // 6
                        || (operation.first == 0x81 && operation.second == 0x02)      // 1
                        || (operation.first == 0x85 && operation.second == 0x02)      // 1
                    )
                    {
                        ;
                    }
                    else
                    {
                        //ESP_LOGI(TAG, "-----");
                        //ESP_LOGI(TAG, "%02X %02X (%i)", operation.first, operation.second, dataLen);
                        // ESP_LOGI(TAG, "CHECK_I: %02X", packetData[len-3]);
                        // ESP_LOGI(TAG, "CHECK_C: %02X", csum);
                       // this->print_data("**", data, dataLen);
                        //ESP_LOGI(TAG, "-----");
                        ;
                    }
                }
                return true;
            }
            return false;
        }

        void DfrobotSen0623Component::print_data(std::string tag, const uint8_t *bytes, size_t len)
        {
            std::string out;
            char buf[5];
            for (size_t i = 0; i < len; i++)
            {
                if (i > 0)
                {
                    out += " ";
                }
                sprintf(buf, "%02X", bytes[i]);
                out += buf;
            }
            //ESP_LOGI(TAG, "%s %s", tag.c_str(), out.c_str());

            out = "";
            for (size_t i = 0; i < len; i++)
            {
                if (i > 2)
                {
                    out += " ";
                }
                if (i > 1 && i < len - 3 && i != 4 && i !=5) {
                  sprintf(buf, "%02X", bytes[i]);
                  out += buf;
                }
            }
            ESP_LOGI(TAG, "%s %s", tag.c_str(), out.c_str());
            
        }

        void DfrobotSen0623Component::setup()
        {
            ESP_LOGI(TAG, "WAITING FOR INIT");
            delay(1000);

            // Send init packet
            this->request(OP_INIT);
            uint8_t result = this->wait_for_packet(OP_INIT);
            if (result != 0xf5) {
                ESP_LOGI(TAG, "WE ARE IN BUSINESS");
                this->status_text_sensor_->publish_state("NA");
                this->movement_text_sensor_->publish_state("NA");
                // Request mode
                this->request(OP_REQ_MODE);
                this->wait_for_packet(OP_REQ_MODE);
                // Reset sensor
                this->request(OP_RST_SENSOR);
                this->wait_for_packet(OP_RST_SENSOR);
                //_d = false;
            } else {
                this->mark_failed();
            }
        }

        bool _pending_update = false;
        // getData(uint8_t con, uint8_t cmd, uint16_t len, uint8_t *senData, uint8_t *retData)
        void DfrobotSen0623Component::update()
        {
            if (_switch_request_rate)
            {
                _pending_update = true;
            }
        }

        void DfrobotSen0623Component::loop()
        {

            // Pending reads
            if (_pending_update) {
                _pending_update = false;
                this->request(OP_REQ_HEART_RATE);
                //this->wait_for_packet(OP_REQ_HEART_RATE);
                this->request(OP_REQ_BREATH_RATE);
                //this->wait_for_packet(OP_REQ_BREATH_RATE);
                this->request(OP_REQ_HUMAN_PRESENCE);
                //this->wait_for_packet(OP_REQ_HUMAN_PRESENCE);
                this->request(OP_REQ_HUMAN_MOVEMENT);
                //this->wait_for_packet(OP_REQ_HUMAN_MOVEMENT);
                this->request(OP_REQ_HUMAN_DISTANCE);
                //this->wait_for_packet(OP_REQ_HUMAN_DISTANCE);
                this->request(OP_REQ_HUMAN_MOVE_RANGE);
                //this->wait_for_packet(OP_REQ_HUMAN_MOVE_RANGE);
            }

            uint8_t packetData[100]; // adjust size as needed
            uint8_t len = this->read_packet(packetData);

            this->process_packet(packetData, len);
            delay(50);
        }

        void DfrobotSen0623Component::dump_config()
        {
            ESP_LOGCONFIG(TAG, "DfrobotSen0623Component");
        }

        void DfrobotSen0623Component::set_switch_request_rate(bool val)
        {
            if (this->request_rate_switch_ != nullptr)
                _switch_request_rate = val;
            this->request_rate_switch_->publish_state(_switch_request_rate);
        }

        void DfrobotSen0623Component::set_switch_hp_led(bool val)
        {
            if (this->request_rate_switch_ != nullptr)
            {
                this->hp_led_switch_->publish_state(val);

                uint8_t data[1];
                if (val)
                {
                    data[0] = 1;
                }
                else
                {
                    data[0] = 0;
                }
                this->forge_packet(0x01, 0x03, data, sizeof(data)); // HP
                // this->forge_packet(0x01, 0x04, data, sizeof(data)); // FALL
            }
        }

    } // namespace dfrobot_sen0623
} // namespace esphome
