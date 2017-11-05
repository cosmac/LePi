/**
 * This file is part of the LePi Project:
 * https://github.com/cosmac/LePi
 *
 * MIT License
 *
 * Copyright (c) 2017 Andrei Claudiu Cosma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// LePi
#include <LeptonCommon.h>
#include <LeptonAPI.h>
#include <LeptonUtils.h>

// C/C++
#include <iostream>


// Open communication with Lepton
bool LePi::OpenConnection()
{
    // Open I2C
    try {
        leptonI2C_connect();
        usleep(kLeptonLoadTime);
        // Read sensor type and prepare config params
        config_ = LeptonCameraConfig(GetType());
    }
    catch (...) {
        std::cerr << "Unable to open connection (I2C) with the sensor." << std::endl;
        return false;
    }

    // Open SPI port
    try {
        leptonSPI_OpenPort(spi_port_, config_.spi_speed);
    }
    catch (...) {
        std::cerr << "Unable to open connection (SPI) with the sensor." << std::endl;
        return false;
    }

    // Prepare frame buffer
    // Note: each image line comes with 4 bytes header
    frame_buffer_.resize((config_.width + 4) * config_.height);

    return true;
}

// Close communication with Lepton
bool LePi::CloseConnection()
{
    try {
        // Close SPI port
        leptonSPI_ClosePort(spi_port_);

        // Close I2C port
        leptonI2C_disconnect();
    }
    catch (...) {
        std::cerr << "Unable to close connection (I2C/SPI) with the sensor." << std::endl;
        return false;
    }

    return true;
}

// Reset communication with Lepton
bool LePi::ResetConnection() {
    bool result_close = CloseConnection();
    usleep(kLeptonResetTime);
    bool result_open = OpenConnection();
    return result_close && result_open;
}

// Reset SPI communication with Lepton
bool LePi::ResetSPIConnection() {

    // Close SPI port
    try {
        leptonSPI_ClosePort(spi_port_);
    }
    catch (...) {
        std::cerr << "Unable to close connection (SPI) with the sensor." << std::endl;
        return false;
    }

    usleep(kLeptonResetTime);

    // Open spi port
    try {
        leptonSPI_OpenPort(spi_port_, config_.spi_speed);
    }
    catch (...) {
        std::cerr << "Unable to open connection (SPI) with the sensor." << std::endl;
        return false;
    }
    return true;
}

// Reboot sensor and reset connection
bool LePi::RebootSensor() {
    leptonI2C_Reboot(); // This function can return a false value even when reboot 
                        // succeed, due to the I2C read after reboot, avoid using 
                        // the returned value for now
    bool result_close = CloseConnection();
    usleep(kLeptonRebootTime);
    bool result_open = OpenConnection();
    return result_close && result_open;
}

// Lepton convert frame from sensor to IR imageU8
void LePi::LeptonUnpackFrame8 (uint8_t *frame) {

    // Prepare buffer pointers
    const uint32_t frame_size{config_.segments_per_frame*config_.segment_size_uint16};
    auto frame_u8 = reinterpret_cast<uint8_t *>(frame_buffer_.data());

    // Compute min and max
    uint16_t minValue = 65535;
    uint16_t maxValue = 0;
    for(uint32_t i = 0; i < frame_size; i++) {

        // Skip the first 2 uint16_t's of every packet, they're 4 bytes header
        if(i % config_.packet_size_uint16 < 2) {
            continue;
        }

        // Flip the MSB and LSB at the last second
        uint8_t temp = frame_u8[i*2];
        frame_u8[i*2] = frame_u8[i*2+1];
        frame_u8[i*2+1] = temp;

        // Check min/max value
        uint16_t value = frame_buffer_[i];
        if(value > maxValue) {
            maxValue = value;
        }
        if(value < minValue) {
            minValue = value;
        }
    }

    // Scale frame range
    auto diff = static_cast<float>(maxValue - minValue);
    float scale = 255.f / diff;
    int idx = 0;
    for(uint32_t i = 0; i < frame_size; i++) {

        // Skip the first 2 uint16_t's of every packet, they're 4 bytes header
        if(i % config_.packet_size_uint16 < 2) {
            continue;
        }

        frame[idx++] = static_cast<uint8_t>((frame_buffer_[i] - minValue) * scale);
    }
}

// Lepton convert frame from sensor to IR imageU16
void LePi::LeptonUnpackFrame16 (uint16_t *frame)
{
    auto src = reinterpret_cast<uint8_t *>(frame_buffer_.data());
    auto dst = reinterpret_cast<uint8_t *>(frame);
    int idx = 0;
    const uint32_t frame_size{config_.segments_per_frame*config_.segment_size};
    for(uint32_t i = 0; i < frame_size; i+=2) {

        // Skip the first 2 uint16_t's of every packet, they're 4 header bytes
        if(i % config_.packet_size < 4) {
            continue;
        }

        // Flip the MSB and LSB
        dst[idx++] = src[i+1];
        dst[idx++] = src[i];
    }
}

// Lepton read segment from sensor
int LePi::LeptonReadSegment(const int max_resets, uint8_t *data_buffer)
{
    int resets{-1};
    const int step{config_.packets_per_read};
    const int bytes_per_SPI_read{step * config_.packet_size};

    for(int j = 0; j < config_.packets_per_segment; j+=step)
    {
        // Try to reach sync first
        if (j == 0) {
            uint8_t packetNumber{255};
            uint8_t discard_packet{0x0F};
            while (packetNumber != 0 || discard_packet == 0x0F) { // while packet id is not 0, or the packet is a discard packet
                ++resets;
                if(resets == max_resets) {
                    return resets;
                }

                usleep(config_.reset_wait_time);
                read(spi_fd, data_buffer, config_.packet_size);
                packetNumber = data_buffer[1];
                discard_packet = data_buffer[0] & 0x0F;
            }
            read(spi_fd, data_buffer + config_.packet_size, (step - 1) * config_.packet_size);
            continue;
        }

        // Check reset counter
        if(resets == max_resets) {
            return resets;
        }

        // Read a packet
        read(spi_fd, data_buffer + j * config_.packet_size, bytes_per_SPI_read);

        // Checks discard packet
        auto discard_packet = data_buffer[j * config_.packet_size] & 0x0F;
        if (discard_packet == 0x0F) {
            usleep(config_.reset_wait_time);
            ++resets;
            j = -step;
            continue;
        }

        // Checks first packet id
        uint8_t packetNumber = data_buffer[j * config_.packet_size + 1];
        if (packetNumber != j) {
            usleep(config_.reset_wait_time);
            ++resets;
            j = -step; // reset just the segment
            continue;
        }

        // Checks last packet id
        /*int last_idx_in_packet = j + step_minus_1;
        uint8_t packetNumber_last = data_buffer[last_idx_in_packet * PACKET_SIZE + 1];
        if (packetNumber_last != last_idx_in_packet) {
            usleep(RESET_WAIT_TIME);
            ++resets;
            j = -step; // reset just the segment
            continue;
        }*/
    }

    return resets;
}

// Read frame from lepton sensor
int LePi::LeptonReadFrame()
{
    // Compute packet index for the packet containing the segment ID
    const int segmentId_packet_idx{config_.segment_number_packet_index * config_.packet_size};

    // Read data packets from lepton over SPI
    auto buffer = reinterpret_cast<uint8_t *>(frame_buffer_.data());
    uint16_t resets{0};
    uint16_t resetsToReboot{0};
    int16_t num_segments{static_cast<int16_t>(config_.segments_per_frame)};
    for(int16_t segment = 0; segment < num_segments; ++segment)
    {
        // Check if reset SPI connection is required
        if(resets > kMaxResetsPerFrame) {
            resets = 0;
            segment = -1;
            LeptonResync(resetsToReboot);
            continue;
        }

        // Read segment
        uint8_t* data_buffer = buffer + segment * config_.segment_size;
        int segment_num_resets = LeptonReadSegment(kMaxResetsPerSegment, data_buffer);
        if (segment_num_resets == kMaxResetsPerSegment) {
            segment = -1;
            LeptonResync(resetsToReboot);
            continue;
        }

        // If Lepton module with more than 1 segment
        if (config_.segments_per_frame > 1) {
            // Checks segment number
            int16_t segmentNumber = (data_buffer[segmentId_packet_idx] >> 4) - 1;
            if (segmentNumber != segment) {
                ++resets;
                segment = -1; // reset all segments
                continue;
            }
        }
    }

    return resets;
}

void LePi::LeptonResync(uint16_t &resetsToReboot) {

    // Re-sync by reboot
    if (resetsToReboot > kMaxResetsBeforeReboot) {
        resetsToReboot = 0;
        if (!RebootSensor()) {
            throw std::runtime_error("Unable to reboot sensor.");
        }
    }
    // Re-sync be connection reset
    else {
        ++resetsToReboot;
        if (!ResetSPIConnection()) {
            throw std::runtime_error("Unable to reset SPI connection.");
        }
    }
}

// Lepton get IR frame from sensor
bool LePi::GetFrame(void *frame, LeptonFrameType type)
{

    // Force reboot if user signaled one
    if (force_reboot_ == true) {
        if (!RebootSensor()) {
            throw std::runtime_error("Unable to reboot sensor.");
        }
        force_reboot_ = false;
    }

    // Read data packets from Lepton over SPI
    // TODO: add a time out, and throw an error when a new frame can't be read
    LeptonReadFrame();

    // Convert Lepton frame to IR frame
    if (type == FRAME_U8) {
        LeptonUnpackFrame8(static_cast<uint8_t *>(frame));
    }
    else if (type == FRAME_U16) {
        LeptonUnpackFrame16(static_cast<uint16_t *>(frame));
    }
    else {
        std::cerr << "Unknown frame type." << std::endl;
        return false;
    }
    count_++;

    return true;
}


// Lepton I2C command
bool LePi::SendCommand(LeptonI2CCmd cmd, void* buffer)
{
    // Run FFC
    bool result{false};
    switch (cmd) {
        case RESET:
        {
            result = ResetConnection();
            break;
        }
        case REBOOT:
        {
            result = true;
            force_reboot_ = true;
            break;
        }
        case FFC:
        {
            result = leptonI2C_FFC();
            break;
        }
        case SENSOR_TEMP_K:
        {
            auto frame_int = static_cast<unsigned int *>(buffer);
            frame_int[0] = leptonI2C_InternalTemp();
            result = frame_int[0] != 0;
            break;
        }
        case SHUTTER_OPEN:
        {
            result = leptonI2C_ShutterOpen();
            break;
        }
        case SHUTTER_CLOSE:
        {
            result = leptonI2C_ShutterClose();
            break;
        }
        default:
        {
            std::cerr << "Unknown I2C command." << std::endl;
            result = false;
            break;
        }
    }

    return result;
}

// Get lepton version
LeptonType LePi::GetType() {

    auto it = kMapLeptonType.find(leptonI2C_SensorNumber());
    if (it != kMapLeptonType.end()) {
        return it->second;
    }

    return LEPTON_UNKNOWN;
}