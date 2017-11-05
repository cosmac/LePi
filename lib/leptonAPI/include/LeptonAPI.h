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
#include <LeptonCommon.h>

// C/C++
#include <atomic>
#include <ctime>
#include <stdint.h>
#include <vector>


/**
 * @brief Custom Lepton sensor API, with support for Lepton v2(2.5) and v3.
 */
class LePi {

public:

    /**
     * @brief Lepton interface constructor/destructor
     */
    LePi() = default;
    LePi(LePi const&) = delete;
    LePi& operator =(LePi const&) = delete;
    virtual ~LePi() = default;

    /**
     * @brief Open communication with Lepton sensor (I2C and SPI)
     * @return true, if connection was successfully open, false otherwise
     */
    bool OpenConnection();

    /**
     * @brief Close communication with Lepton sensor (I2C and SPI)
     * @return true, if connection was successfully close, false otherwise
     */
    bool CloseConnection();

    /**
     * @brief Reset SPI and I2C communication with Lepton sensor
     * @return true, if connection was successfully reset, false otherwise
     */
    bool ResetConnection();

    /**
     * @brief Reset SPI connection with the Lepton sensor
     * @return true, if SPI connection was successfully reset, false otherwise
     */
    bool ResetSPIConnection();

    /**
     * @brief Reboot sensor and re-establish communication
     * @return true, if reboot was successfully, false otherwise
     */
    bool RebootSensor();

    /**
     * @brief Send I2C command to the sensor
     * @param cmd     I2C known command, see Common.h
     * @param buffer  Data buffer, some I2C commands return data (buffer should
     *                have proper size to fit all the returned data)
     * @return true, if I2C command succeed, false otherwise
     */
    bool SendCommand(LeptonI2CCmd cmd, void* buffer);

    /**
     * @brief Get new frame from sensor
     * @param frame  Buffer to the frame data, must be allocated with proper size
     * @param type   Desired frame pixel depth (U8 or U16)
     * @return true, if frame was successfully read and frame type requested is
     *         valid, false otherwise
     */
    bool GetFrame(void *frame, LeptonFrameType type);

    /**
     * @brief Get Lepton type from sensor info
     * @return Lepton version
     */
    LeptonType GetType();

protected:
    /**
     * @brief Read 1 frame segment over SPI
     * @return Number of resets during a segment read
     */
    int LeptonReadSegment(const int max_resets, uint8_t* data_buffer);

    /**
     * @brief Read 1 frame over SPI
     * @return Number of resets during the complete frame read
     */
    int LeptonReadFrame();

    /**
     * @brief Tries to re-sync SPI communication with sensor
     * @param resetsToReboot  Number of resets until a reboot is required
     * @throw Runtime error if unable to re-sync with Lepton
     */
    void LeptonResync(uint16_t &resetsToReboot);

    /**
     * @brief Unpack latest received frame, keep IR full range
     */
    void LeptonUnpackFrame16 (uint16_t *frame );

    /**
     * @brief Unpack latest received frame, and scale IR range to [0, 255], using
     *        frame's min and max IR values
     */
    void LeptonUnpackFrame8 (uint8_t *frame);

private:
    LeptonCameraConfig config_;
    std::vector<uint16_t> frame_buffer_;
    int count_{0};
    int spi_port_{0};
    std::atomic<bool> force_reboot_;
};
