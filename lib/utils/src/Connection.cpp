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

// C/C++
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

void GetIP (std::string& ipV4, std::string& ipV6) {
    
    std::string lo_ipV4{""}, lo_ipV6{""};
    struct ifaddrs* ifAddrStruct{nullptr};
    struct ifaddrs* ifa{nullptr};
    void* tmpAddrPtr{nullptr};

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        
        // check if it is IP4
        if (ifa->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
            if (strcmp(ifa->ifa_name, "eth0") == 0) {
            	ipV4 = addressBuffer;
            }
            if (strcmp(ifa->ifa_name, "lo") == 0) {
            	lo_ipV4 = addressBuffer;
            }
        }
        // check if it is IP6
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //printf("%s IP6 Address %s\n", ifa->ifa_name, addressBuffer); 
            if (strcmp(ifa->ifa_name, "eth0") == 0) {
            	ipV6 = addressBuffer;
            }
            if (strcmp(ifa->ifa_name, "lo") == 0) {
            	lo_ipV6 = addressBuffer;
            }
        } 
    }
    
    if (ifAddrStruct != nullptr) {
        freeifaddrs(ifAddrStruct);
    }
    
    if (ipV4.empty()) {
        ipV4 = lo_ipV4;
        ipV6 = lo_ipV6;
    }
}

bool ConnectSubscriber(int port_number,
                       std::string ip_address,
                       int& socketHandle) {

    // If no IP address provided, use RPi IP address
    if (ip_address.empty()) {
        std::string ipV4{""};
        std::string ipV6{""};
        GetIP(ipV4, ipV6);
        std::cout << "RPi IPv4: " << ipV4 << std::endl;
        std::cout << "RPi IPv6: " << ipV6 << std::endl;
        ip_address = ipV4;
    }

    // Create socket
    if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Server fail to create the socket." << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketHandle);
        return false;
    }

    // Load system information into socket data structures
    struct sockaddr_in remoteSocketInfo;
    bzero(&remoteSocketInfo, sizeof(sockaddr_in));
    remoteSocketInfo.sin_family = AF_INET;
    remoteSocketInfo.sin_addr.s_addr = inet_addr(ip_address.c_str());
    remoteSocketInfo.sin_port = htons((u_short)port_number);


    // Establish the connection with the server
    if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0)
    {
        std::cerr << "Client fail to connect to the server." << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketHandle);
        return false;
    }

    return true;
}

bool ConnectPublisher(int port_number,
                      std::string ip_address,
                      int& socketConnection) {

    // If no IP address provided, use RPi IP address
    if (ip_address.empty()) {
        std::string ipV4{""};
        std::string ipV6{""};
        GetIP(ipV4, ipV6);
        std::cout << "RPi IPv4: " << ipV4 << std::endl;
        std::cout << "RPi IPv6: " << ipV6 << std::endl;
    }
    
    // Create socket
    int socketHandle;
    if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Server fail to create the socket." << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketHandle);
        return false;
   }

    // Load system information into socket data structures
    struct sockaddr_in socketInfo;
    bzero(&socketInfo, sizeof(sockaddr_in));  // Clear structure memory
    socketInfo.sin_family = AF_INET;
    socketInfo.sin_addr.s_addr = INADDR_ANY;
    socketInfo.sin_port = htons((u_short)port_number);

    // Bind the socket to a local socket address
    if( bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(socketInfo)) < 0) {
        std::cerr << "Server fail to bind the socket." << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketHandle);
        return false;
    }

    // Wait for a client to connect
    if (listen(socketHandle, 1) < 0) {
        std::cerr << "Server fail to listen for a connection." << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketHandle);
        return false;
    }

    // Accept incoming connection
    if( (socketConnection = accept(socketHandle, NULL, NULL)) < 0)
    {
        std::cerr << "Server fail to accept client connection." << std::endl;
        std::cerr << "Error: " << strerror(errno) << std::endl;
        close(socketHandle);
        return false;
    }
    
    close(socketHandle);
    return true;
}
