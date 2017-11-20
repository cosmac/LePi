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
#include <ConnectionCommon.h>

// C/C++
#include <cstring>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <iostream>

/**
 * @brief Get RPi IP address
 * @param ipV4
 * @param ipV6
 */
void GetIP (std::string& ipV4, std::string& ipV6);

/**
 * @brief Open TCP/IP communication with Publisher
 * @param port_number   Publisher port number
 * @param ip_address    Publisher IP address. If empty, current device IP address
 *                      will be used and the publisher must run on the same machine
 * @param socketHandle  Created socket handle
 * @return True, if connection successfully open, false otherwise
 */
bool ConnectSubscriber(int port_number,
                       std::string ip_address,
                       int& socketHandle);

/**
 * @brief Open TCP/IP communication
 * @param port_number   Desired port number
 * @param ip_address    Desired IP address. If empty, the publisher will listen for
 *                      any incoming connection from all addresses
 * @param socketHandle  Created socket handle
 * @return True, if connection successfully open, false otherwise
 */
bool ConnectPublisher(int port_number,
                      std::string ip_address,
                      int& socketHandle);

/**
 * @brief Receive a message from the connected socket
 * @tparam T            Message type
 * @param socketHandle  Socket connection address
 * @param msg           Receive message
 */
template <typename T>
inline void ReceiveMessage(int socketHandle,
                           T& msg) {

    size_t data_size{0};
    do
    { // wait for data to be available
        ioctl(socketHandle, FIONREAD, &data_size);
    } while (data_size < sizeof(T));
    auto rc = recv(socketHandle, &msg, sizeof(T), 0);

    // Check if connection is still open
    if (rc == -1) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        throw std::runtime_error("Connection lost.");
    }
}

/**
 * @brief Send a message to the connected socket
 * @tparam T            Message type
 * @param socketHandle  Socket connection address
 * @param msg           Message content
 */
template <typename T>
inline void SendMessage(int socketHandle,
                        T& msg) {

    auto sd = send(socketHandle, &msg, sizeof(T), 0);
    if (sd == -1) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
        throw std::runtime_error("Connection lost.");
    }
}