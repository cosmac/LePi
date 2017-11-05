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
#include <LeptonCamera.h>

// Third party
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// C/C++
#include <iostream>
#include <unistd.h>
#include <chrono>

/**
 * @brief Sample app for streaming IR videos using LePi parallel interface
 */
int main()
{    
    // Open camera connection
    LeptonCamera cam;
    cam.start();
    
    // Define frame
    std::vector<uint8_t> frame(cam.width() * cam.height());
    cv::Mat img(cam.height(), cam.width(), CV_8UC1, frame.data());
    
    // Stream frames
    int frame_nb{0};
    auto start_time = std::chrono::system_clock::now();
    while (true) {
    
        // Frame request
        if (cam.hasFrame()) {
            cam.getFrameU8(frame);
            ++frame_nb;
        }
        
        // Display
        cv::imshow("Lepton", img);
        int key = cv::waitKey(10);
        if (key == 27) { // Press Esc to exit
            break;
        }
        else if (key == 'f') {
            cam.sendCommand(FFC, nullptr);
        }
        else if (key == 'r') {
            cam.sendCommand(REBOOT, nullptr);
        }
        else if (key == 't') {
            int temperature{0};
            // Received temperature is in Kelvin and scaled by 100
            cam.sendCommand(SENSOR_TEMP_K, &temperature);
            std::cout << "Sensor temperature: " 
                      << temperature/100.0 << " K" << std::endl;
        }
        else if (key == 'h') {
            std::cout << "Help: " << std::endl
                      << "\t- f: run flat filed correction (FFC)" << std::endl
                      << "\t- r: run sensor reboot" << std::endl
                      << "\t- t: show sensor temperature" << std::endl
                      << "\t- h: show help" << std::endl;
        }
        
        // Runtime
        auto end_time = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast
            <std::chrono::seconds>(end_time - start_time);
        if (elapsed.count() > 1.0) {
            double fps = static_cast<double>(frame_nb) / static_cast<double>(elapsed.count());
            std::cout << "FPS: " << fps << std::endl;
            start_time = end_time;
            frame_nb = 0;
        }
    }

    // Release sensor
    cam.stop();

    return EXIT_SUCCESS;
}
