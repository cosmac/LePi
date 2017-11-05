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

// Third party
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// C/C++
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <chrono>


/**
 * @brief Sample app for grabbing IR frames using LePi serial interface
 */
int main()
{
    // Init sensor
    LePi lePi;
    if (!lePi.OpenConnection()) {
        std::cerr << "Unable to open communication with the sensor" << std::endl;
        exit (EXIT_FAILURE);
    }

    // Frame buffer
    LeptonType lp_type = lePi.GetType();
    LeptonCameraConfig lp_config(lp_type);
    std::vector<uint8_t> frame(lp_config.height * lp_config.width);
    cv::Mat img(lp_config.height, lp_config.width, CV_8UC1, frame.data());
    
    // Grab frames
    int count{0};
    auto tStart = std::chrono::system_clock::now();
    while (true)
    {
        // Frame request
        lePi.GetFrame(frame.data(), FRAME_U8);
        
        // Display
        cv::imshow("Lepton", img);
        int key = cv::waitKey(10);
        if (key == 27) { // Press Esc to exit
            break;
        }
        
        // Runtime
        auto tEnd = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(tEnd - tStart);
        if (elapsed.count() > 1.0) {
            double fps = count / elapsed.count();
            std::cout << "FPS: " << fps << std::endl;
            tStart = tEnd;
            count = 0;
        }
        count++;
    }

    // Release sensor
    if (!lePi.CloseConnection()) {
        std::cerr << "Unable to close communication with the sensor" << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
