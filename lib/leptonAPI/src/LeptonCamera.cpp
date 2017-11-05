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
#include <LeptonUtils.h>
#include <LeptonAPI.h>
#include <LeptonCamera.h>

// C/C++
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>


LeptonCamera::LeptonCamera()
        : grabber_thread_(),
          run_thread_{false},
          has_frame_{false},
          lePi_(),
          sensor_temperature_{0.0} {

    // Open communication with the sensor
    if (!lePi_.OpenConnection()) {
        std::cerr << "Unable to open communication with the sensor" << std::endl;
        throw std::runtime_error("Connection failed.");
    }

    // Check lepton type
    lepton_type_ = lePi_.GetType();
    if (LEPTON_UNKNOWN == lepton_type_) {
        throw std::runtime_error("Unknown lepton type.");
    }

    // Prepare buffers
    lepton_config_ = LeptonCameraConfig(lepton_type_);
    frame_to_read_.resize(lepton_config_.width * lepton_config_.height);
    frame_to_write_.resize(lepton_config_.width * lepton_config_.height);
};

LeptonCamera::~LeptonCamera() {
    // Stop thread
    stop();

    // Close communication with the sensor
    if (!lePi_.CloseConnection()) {
        std::cerr << "Unable to close communication with the sensor" << std::endl;
    }
};

void LeptonCamera::start() {

    // Avoid starting the thread if already runs
    if (false == run_thread_) {
        run_thread_ = true;
        grabber_thread_ = std::thread(&LeptonCamera::run, this);
    }
}

void LeptonCamera::stop() {
    // Stop the thread only if there is a thread running
    if (true == run_thread_) {
        run_thread_ = false;
        if (grabber_thread_.joinable()) {
            grabber_thread_.join();
        }
    }
}

void LeptonCamera::run() {

    while (run_thread_) {

        // Get new frame
        try {
            sensor_temperature_ = leptonI2C_InternalTemp();
            if (!lePi_.GetFrame(frame_to_write_.data(), FRAME_U16)) {
                continue;
            }
        }
        catch (...) {
            lePi_.RebootSensor();
            continue;
        }

        // Lock resources and swap buffers
        lock_.lock();
        std::swap(frame_to_write_, frame_to_read_);
        has_frame_ = true;
        lock_.unlock();
    }
}

void LeptonCamera::getFrameU8(std::vector<uint8_t>& frame) {

    // Resize output frame
    frame.resize(frame_to_read_.size());

    // Lock resources
    lock_.lock();

    // Find frame min and max
    uint16_t minValue = 65535;
    uint16_t maxValue = 0;
    for(size_t i = 0; i < frame_to_read_.size(); i++) {
        if (frame_to_read_[i] > maxValue) {
            maxValue = frame_to_read_[i];
        }
        if (frame_to_read_[i] < minValue) {
            minValue = frame_to_read_[i];
        }
    }
    // Scale frame range and copy to output
    float scale = 255.f / static_cast<float>(maxValue - minValue);
    for(size_t i = 0; i < frame_to_read_.size(); i++) {
        frame[i] = static_cast<uint8_t>((frame_to_read_[i] - minValue) * scale);
    }
    has_frame_ = false;

    // Release resources
    lock_.unlock();
}

void LeptonCamera::getFrameU16(std::vector<uint16_t>& frame) {
    // Lock resources
    lock_.lock();

    std::copy(frame_to_read_.begin(), frame_to_read_.end(), frame.begin());
    has_frame_ = false;

    // Release resources
    lock_.unlock();
}
   
bool LeptonCamera::sendCommand(LeptonI2CCmd cmd, void* buffer) {
    return lePi_.SendCommand(cmd, buffer);
}