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

// Client message
enum STREAMER_MSG { FRAME_REQUEST, // Request a frame [IN/OUT]
                    I2C_CMD,      // Sends an I2C command [IN/OUT]
                    UNKNOWN_MSG    // Response msg for an unknown [OUT]
                  };

// Server response
enum SERVER_RESP { FRAME_READY,   // The response contains a valid frame
                   NO_FRAME,      // No frame ready to be read [We can avoid this by repeating last frame]
                   I2C_SUCCEED,   // I2C command was applied with success
                   I2C_FAILED,    // I2C command failed
                   RESEND         // If the client message was something unknown, ask for a resend
                 };
