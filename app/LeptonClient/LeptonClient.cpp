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

// Third party
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// C/C++
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>


// User defines
#define DPRINTF //printf


/**
 * Sample app for streaming video over the local network (TCP)
 */
int main() {

    // Create socket
    const int kPortNumber{5995};
    const std::string kIPAddress{""};
    int socketHandle;
    if (!ConnectSubscriber(kPortNumber, kIPAddress, socketHandle)) {
        std::cerr << "Unable to open connection." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Prepare frame request message. For simplicity, we chose to ask for
    // U8 frames, so the image is ready for display. For full pixel depth use U16.
    RequestMessage req_msg;
    req_msg.req_type = REQUEST_FRAME;
    req_msg.req_cmd = CMD_FRAME_U8;

    // Stream data
    cv::Mat ir_img;
    while (true) {

    	//  Send Request
        SendMessage(socketHandle, req_msg);

        // Receive response
        ResponseMessage resp_msg;
        ReceiveMessage(socketHandle, resp_msg);

        // Check response header
        switch (resp_msg.req_type) {

            case REQUEST_FRAME:
            {
                DPRINTF("CLIENT -- RECV -- FRAME_REQUEST response. \n");
                ir_img = cv::Mat(resp_msg.height, resp_msg.width, CV_8UC1);
                memcpy(ir_img.data, resp_msg.frame,
                       resp_msg.width * resp_msg.height);
                break;
            }
            case REQUEST_I2C:
            {
                DPRINTF("CLIENT -- RECV -- I2C_CMD response. \n");
                break;
            }
            case REQUEST_UNKNOWN:
            {
                std::cerr << "CLIENT -- Server did not recognize your request."
                          << std::endl;
                exit(EXIT_FAILURE);
            }
            default:
            {
                std::cerr << "CLIENT -- Unable to decode server message."
                          << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        // Show image
        imshow("IR Img", ir_img);
        int key = cvWaitKey(5);
        if (key == 27) { // Press ESC to exit
            break;
        }

    }

    // Close connection
    req_msg.req_type = REQUEST_EXIT;
    req_msg.req_cmd = CMD_VOID;
    SendMessage(socketHandle, req_msg);
    DPRINTF("CLIENT -- Closing Connection...");
    close(socketHandle);
    DPRINTF(" Closed ! \n");

    return EXIT_SUCCESS;
}
