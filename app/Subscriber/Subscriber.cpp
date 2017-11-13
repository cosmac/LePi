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
int main() {

    // Create socket
    const int kPortNumber{5995};
    const std::string kIPAddress{""};
    int socketHandle;
    if (!ConnectSubscriber(kPortNumber, kIPAddress, socketHandle)) {
        std::cerr << "Unable to open connection." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Send data
    constexpr int width{160};
    constexpr int height{120};
    int pckg_size = 10 + width * height * 2;// Header + IR img(80*60*2)
    char img[pckg_size];
    char msg[] = {FRAME_REQUEST, FRAME_U8}; // Header(MSG+Response=2)
    int rc = 0;
    cv::Mat ir_img (height, width, CV_8UC1);

    int count{0};
    while (true) {

    	//  Send Request
    	DPRINTF("[%d]CLIENT -- SEND -- Sending message... ", count);
        int sd = send(socketHandle, msg, sizeof(msg), 0);
        if (sd == -1) {
            std::cerr << "[" << count << "]CLIENT -- CONNECTION -- Lost." << std::endl;
            std::cerr << "Error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        DPRINTF(" Message sent! \n");

        // Receive response
        int data_size{0};
        do
        { // wait for data to be available
        	ioctl(socketHandle, FIONREAD, &data_size);
        } while (data_size < pckg_size);
        rc = recv(socketHandle, img, sizeof(img), 0);
        DPRINTF("[%d]CLIENT -- RECV -- Number of bytes read: %d \n", count, rc);

        // Check if connection is still open
        if (rc == -1) {
            std::cerr << "[" << count << "]CLIENT -- CONNECTION -- Lost." << std::endl;
            std::cerr << "Error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        // Check response header
        if (img[0] == FRAME_REQUEST) {
        	DPRINTF("[%d]CLIENT -- RECV -- FRAME_REQUEST response. \n", count);
        	memcpy(ir_img.data, img+10, width * height);
        }
        else if (img[0] == I2C_CMD) {
        	DPRINTF("[%d]CLIENT -- RECV -- I2C_CMD response. \n", count);
        }
        else if (img[0] == UNKNOWN_MSG) {
            std::cerr << "[" << count << "]CLIENT -- Server did not recognize your request." << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            std::cerr << "[" << count << "]CLIENT -- Unable to decode server message." << std::endl;
            exit(EXIT_FAILURE);
        }

        // Show image
        imshow("IR Img", ir_img);
        int key = cvWaitKey(5);
        if (key == 27) {
            break;
        }

        count++;
    }

    return EXIT_SUCCESS;
}
