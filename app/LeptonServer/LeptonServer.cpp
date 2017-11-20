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
#include <Connection.h>
#include <ConnectionCommon.h>
#include <LeptonCommon.h>
#include <LeptonCamera.h>

// C/C++
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

// User defines
#define DPRINTF //printf

/**
 * Sample app for streaming video over the local network (TCP)
 */
int main()
{
    
    // Create socket
    const int kPortNumber{5995};
    const std::string kIPAddress{""};
    int socketConnection;
    if (!ConnectPublisher(kPortNumber, kIPAddress, socketConnection)) {
        std::cerr << "Unable to create connection." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Open camera connection
    LeptonCamera lePi;
    lePi.start();

    // Send data
    const uint32_t width{lePi.width()};
    const uint32_t height{lePi.height()};
    std::vector<uint8_t> imgU8(width * height);
    std::vector<uint16_t> imgU16(width * height);

    uint64_t frame_id{0};
    while (true) {
    
        //  Receive Request
        RequestMessage req_msg;
        ReceiveMessage(socketConnection, req_msg);

        // Frame request msg
        ResponseMessage resp_msg;
        switch (req_msg.req_type) {

            case REQUEST_FRAME:
            {
                DPRINTF("[%d]SERVER -- RECV -- Message: FRAME_REQUEST \n", frame_id);
                resp_msg.req_type = REQUEST_FRAME;
                resp_msg.sensor_temperature = lePi.SensorTemperature();
                if (req_msg.req_cmd == CMD_FRAME_U8) {
                    lePi.getFrameU8(imgU8);
                    memcpy(resp_msg.frame, imgU8.data(), imgU8.size());
                    resp_msg.bpp = 1;
                } else {
                    lePi.getFrameU16(imgU16);
                    memcpy(resp_msg.frame, imgU16.data(), imgU16.size() * 2);
                    resp_msg.bpp = 2;
                }
                resp_msg.req_status = STATUS_FRAME_READY;
                resp_msg.height = height;
                resp_msg.width = width;
                resp_msg.frame_id = frame_id;
                break;
            }
            case REQUEST_I2C:
            {
                DPRINTF("[%d]SERVER -- RECV -- Message: I2C_CMD \n", frame_id);
                resp_msg.req_type = REQUEST_I2C;
                resp_msg.frame_id = frame_id;
                if (lePi.sendCommand(static_cast<LeptonI2CCmd>(req_msg.req_cmd),
                                     resp_msg.frame)) {
                    resp_msg.req_status = STATUS_I2C_SUCCEED;
                }
                else {
                    resp_msg.req_status = STATUS_I2C_FAILED;
                }
                break;
            }
            default :
            {
                DPRINTF("[%d]SERVER -- RECV -- Message: UNKNOWN_MSG \n", frame_id);
                resp_msg.req_type = REQUEST_UNKNOWN;
                resp_msg.req_status = STATUS_RESEND;
            }
        }

        // Send response
        SendMessage(socketConnection, resp_msg);

        frame_id++;
   }

    // Release sensors
    lePi.stop();

    // Close connection
    DPRINTF("[%d]SERVER -- Closing Connection...", frame_id);
    close(socketConnection);
    DPRINTF(" Closed ! \n");

    return EXIT_SUCCESS;

}
