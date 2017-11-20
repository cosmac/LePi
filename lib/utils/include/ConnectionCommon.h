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
#include <stdint.h>
#include <stdlib.h>

constexpr size_t kMaxWidth{160};
constexpr size_t kMaxHeight{120};
constexpr size_t kMaxBytesPerPixel{2};

// Request message type
enum RequestType {
    REQUEST_FRAME,   // Request a frame
    REQUEST_I2C,     // Request an I2C command
    REQUEST_EXIT,    // Request app exit
    REQUEST_UNKNOWN  // Unknown request
};

// Request message command
enum RequestCmd {
    // Frame request cmd
    CMD_FRAME_U8,
    CMD_FRAME_U16,
    // I2C request cmd
    CMD_I2C_FFC,
    CMD_I2C_SENSOR_TEMPERATURE,
    CMD_VOID
};

// Request message status
enum RequestStatus {
    STATUS_FRAME_READY,   // The response contains a valid frame
    STATUS_NO_FRAME,      // No frame ready to be read [We can avoid this by repeating last frame]
    STATUS_I2C_SUCCEED,   // I2C command was applied with success
    STATUS_I2C_FAILED,    // I2C command failed
    STATUS_RESEND         // If the client message was something unknown, ask for a resend
};

// Subscriber uses this message to ask publisher for data
struct RequestMessage {
    RequestType req_type{REQUEST_UNKNOWN};
    RequestCmd req_cmd{CMD_VOID};
};

// Publisher uses this message in response to the subscriber request
struct ResponseMessage {
    RequestType req_type{REQUEST_UNKNOWN};
    RequestStatus req_status{STATUS_RESEND};
    uint32_t width{0};
    uint32_t height{0};
    uint32_t bpp{0};
    uint64_t frame_id{0};
    char frame[kMaxWidth * kMaxHeight * kMaxBytesPerPixel];
    double sensor_temperature{0.0};
};
