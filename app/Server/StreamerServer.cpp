/**
 * Sample app for streaming video over the local network (TCP)
 */

// LePi
#include <ServerCommon.h>
#include <LeptonCommon.h>
#include <LeptonCamera.h>

// C/C++
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

// User defines
#define DPRINTF //printf

void getIP (std::string& ipV4, std::string& ipV6) {
    
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
        } 
    }
    
    if (ifAddrStruct != nullptr) {
        freeifaddrs(ifAddrStruct);
    }
}

//============================================================================
// Main application
//============================================================================
int main()
{
    std::string ipV4{""};
    std::string ipV6{""};
    getIP(ipV4, ipV6);
    printf("IPv4: %s \n", ipV4.c_str());
    printf("IPv6: %s \n", ipV6.c_str());
    const int portNumber{5995};
    
    // Create socket
    int socketHandle;
    if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Server fail to create the socket." << endl;
        cerr << "Error: " << strerror(errno) << endl;
        close(socketHandle);
        exit(EXIT_FAILURE);
   }

    // Load system information into socket data structures
    struct sockaddr_in socketInfo;
    bzero(&socketInfo, sizeof(sockaddr_in));  // Clear structure memory
    socketInfo.sin_family = AF_INET;
    socketInfo.sin_addr.s_addr = INADDR_ANY;
    socketInfo.sin_port = htons((u_short)portNumber);


    // Bind the socket to a local socket address
    if( bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(socketInfo)) < 0)
    {
        cerr << "Server fail to bind the socket." << endl;
        cerr << "Error: " << strerror(errno) << endl;
        close(socketHandle);
        exit(EXIT_FAILURE);
    }


    // Wait for a client to connect
    if (listen(socketHandle, 1) < 0)
    {
        cerr << "Server fail to listen for a connection." << endl;
        cerr << "Error: " << strerror(errno) << endl;
        close(socketHandle);
        exit(EXIT_FAILURE);
    }


    // Accept incoming connection
    int socketConnection;
    if( (socketConnection = accept(socketHandle, NULL, NULL)) < 0)
    {
        cerr << "Server fail to accept client connection." << endl;
        cerr << "Error: " << strerror(errno) << endl;
        close(socketHandle);
        exit(EXIT_FAILURE);
    }
    close(socketHandle);

    // Open camera connection
    LeptonCamera lePi;
    lePi.start();

    // Send data
    const uint32_t width{lePi.width()};
    const uint32_t height{lePi.height()};
    const uint32_t bpp{2};
    const uint32_t header_size{2 + 4 + 4}; // Header(MSG(1) + CMD(1) + To(4) + SensorTemp(4)
    std::vector<uint8_t> imgU8(width * height);
    std::vector<uint16_t> imgU16(width * height);
    std::vector<uint8_t> frame(header_size + width * height * bpp); // Header + IR img
    char msg[2]; // Header(MSG+Response=2)
    int rc = 0;
    memset(msg, 1, sizeof(msg));

    int count = 0;
    while (true)
    {
        ////////////////////////////////////////////////////////////////////////
        ///  Receive Request
        ////////////////////////////////////////////////////////////////////////
        rc = recv(socketConnection, msg, sizeof(msg), 0);
        DPRINTF("[%d]SERVER -- RECV -- Number of bytes read: %d \n", count, rc);

         // Check if connection is still open
         if (rc == -1)
         {
             cerr << "[" << count << "]SERVER -- CONNECTION -- Lost." << endl;
             cerr << "Error: " << strerror(errno) << endl;
             exit(EXIT_FAILURE);
         }

         // Frame request msg
         if (msg[0] == FRAME_REQUEST)
         {
             DPRINTF("[%d]SERVER -- RECV -- Message: FRAME_REQUEST \n", count);
             frame[0] = FRAME_REQUEST;
             //((float *)(frame.data()+6))[0] = lePi.SensorTemperature();
             if (msg[1] == FRAME_U8) {
                 lePi.getFrameU8(imgU8);
                 memcpy(frame.data()+10, imgU8.data(), imgU8.size()*bpp);
             }
             else {
                 lePi.getFrameU16(imgU16);
                 memcpy(frame.data()+10, imgU16.data(), imgU16.size()*bpp);
             }
             frame[1] = FRAME_READY;
        }
        else if (msg[0] == I2C_CMD)
        { // I2C command
            DPRINTF("[%d]SERVER -- RECV -- Message: I2C_CMD \n", count);
            frame[0] = I2C_CMD;
            if (lePi.sendCommand((LeptonI2CCmd)(msg[1]), frame.data()+2)) {
                frame[1] = I2C_SUCCEED;
            }
            else {
                frame[1] = I2C_FAILED;
            }
        }
        else
        { // UNKNOWN Request
            DPRINTF("[%d]SERVER -- RECV -- Message: UNKNOWN_MSG \n", count);
            frame[0] = UNKNOWN_MSG;
            frame[1] = VOID;
        }

        ////////////////////////////////////////////////////////////////////////
        /// Send response
        ////////////////////////////////////////////////////////////////////////
        DPRINTF("[%d]SERVER -- SEND -- Sending message response... ", count);
        int sd = send(socketConnection, frame.data(), frame.size(), 0);
        if (sd == -1)
        {
            cerr << "[" << count << "]SERVER -- CONNECTION -- Lost." << endl;
            cerr << "Error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        DPRINTF(" Message sent! \n");

        count++;
   }

    // Release sensors
    lePi.stop();

    // Close connection
    DPRINTF("[%d]SERVER -- Closing Connection...", count);
    close(socketConnection);
    DPRINTF(" Closed ! \n");

    return EXIT_SUCCESS;

}
