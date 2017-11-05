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

// LePi
#include <LeptonAPI.h>
#include <LeptonCommon.h>

// Third party
#include <bcm2835.h>

// C/C++
#include <vector>
#include <cstdint>
#include <thread>
#include <atomic>
#include <mutex>

/**
 * @brief Lepton parallel camera interface based on a grabber thread that
 *        ensures that all frames are read from the sensors
 */
class LeptonCamera {
public:

    /**
     * @brief Lepton camera constructor/destructor
     * @throw Runtime error when installed Lepton module is not recognized
     */
    LeptonCamera();
    // Delete copy constructor and copy operator
    LeptonCamera(LeptonCamera const&) = delete;
    LeptonCamera& operator =(LeptonCamera const&) = delete;
    virtual ~LeptonCamera();

    /**
     * @brief Start camera grabber thread
     */
    void start();
    
    /**
     * @brief Stop camera grabber thread
     */
    void stop();

    /**
     * @brief Sends an I2C command to the Lepton sensor
     * @param cmd     I2C known command, see LeptonCommon.h
     * @param buffer  Data buffer, some I2C commands return data (buffer should
     *                have proper size to fit all the returned data)
     * @return true, if I2C command succeed, false otherwise
     */
    bool sendCommand(LeptonI2CCmd cmd, void* buffer);

    /**
     * @brief Lepton frame accessors
     */
    inline bool hasFrame() const {return has_frame_; }
    void getFrameU8(std::vector<uint8_t>& frame);
    void getFrameU16(std::vector<uint16_t>& frame);
    
    /**
     * @brief Lepton sensor specification accessors
     */
    inline double SensorTemperature() const { return sensor_temperature_; }
    inline LeptonType LeptonVersion() const { return lepton_type_; }
    inline uint32_t width() const { return lepton_config_.width; }
    inline uint32_t height() const { return lepton_config_.height; }

private:
    /**
     * @brief Camera grabber (runs in a parallel thread)
     */
    void run();
    
    // Camera grabber thread
    std::thread grabber_thread_;
    std::atomic<bool> run_thread_;
    std::mutex lock_;

    // IR frame double buffer
    std::vector<uint16_t> frame_to_read_;
    std::vector<uint16_t> frame_to_write_;
    std::atomic<bool> has_frame_;
    
    // Sensor info
    LePi lePi_;
    LeptonType lepton_type_;
    LeptonCameraConfig lepton_config_;
    double sensor_temperature_;
};
