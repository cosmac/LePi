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

#pragma once

// C/C++
#include <cstdint>
#include <iostream>
#include <map>


// Accepted Lepton types
enum LeptonType {
    LEPTON2,	// Lepton 2.X
    LEPTON3,	// Lepton 3.X
    LEPTON_UNKNOWN
};
const std::map<int, LeptonType> kMapLeptonType {
    {2, LEPTON2}, {3, LEPTON3}
};


 // Lepton frame types
enum LeptonFrameType { 
    FRAME_U8,    // 8 bit per pixel
    FRAME_U16    // 16 bit per pixel
};


// Lepton communication timing parameters
constexpr uint16_t kMaxResetsPerSegment{500};   // packet resets
constexpr uint16_t kMaxResetsPerFrame{40};      // segment resets
constexpr uint16_t kMaxResetsBeforeReboot{2};   // frame resets
constexpr uint32_t kLeptonLoadTime{200000};     // 0.2 s = 200 ms = 200000 us
constexpr uint32_t kLeptonResetTime{300000};    // 0.3 s = 300 ms = 300000 us
constexpr uint32_t kLeptonRebootTime{1500000};  // 1.5 s = 1500 ms = 1500000 us


// Lepton camera specification, based on the lepton version/type
struct LeptonCameraConfig {
    uint16_t packet_size;           // SPI packet size in bytes
    uint16_t packet_size_uint16;    // SPI packet size in words
    uint16_t packets_per_segment;   // SPI packets per segment
    uint16_t packets_per_read;      // number of SPI packets read in one SPI read call

    uint32_t segment_size;                  // segment size in bytes
    uint32_t segment_size_uint16;           // segment size in words
    uint16_t segments_per_frame;            // number of segments per frame
    uint16_t segment_number_packet_index;   // packet index for the packet containing the segment number
    uint32_t reset_wait_time;               // wait time in microseconds for each reset

    uint32_t spi_speed;     // SPI speed
    uint16_t width;         // Frame width
    uint16_t height;        // Frame height

    LeptonCameraConfig() = default;
    LeptonCameraConfig(LeptonType lp_t) {
        packet_size = 164;
        packet_size_uint16 = packet_size / 2;
        packets_per_segment = 60;
        packets_per_read = 1;   // must be a divisor of packets_per_segment

        segment_size = packet_size * packets_per_segment;
        segment_size_uint16 = packet_size_uint16 * packets_per_segment;

        if (lp_t == LEPTON2) {
            segments_per_frame = 1;
            segment_number_packet_index = 0;
            reset_wait_time = 1000;
            spi_speed = 10000000; //10MHz
            width = 80;
            height = 60;
        }
        else if (lp_t == LEPTON3) {
            segments_per_frame = 4;
            segment_number_packet_index = 20;
            reset_wait_time = 1000;
            spi_speed = 32000000; //32MHz
            width = 160;
            height = 120;
        }
        else {
            std::cerr << "Error: Unknown Lepton version.";
            throw std::runtime_error("Unknown Lepton version.");
        }
    };
};


// Lepton I2C commands
enum LeptonI2CCmd {
    RESET,          // Sensor connection reset
    REBOOT,         // Sensor reboot
    FFC,            // Flat Field Correction
    SENSOR_TEMP_K,  // Get sensor internal temperature in kelvin
    SHUTTER_OPEN,   // Open camera shutter
    SHUTTER_CLOSE,  // Close camera shutter
    VOID            // No command (just for frame request msg)
};
