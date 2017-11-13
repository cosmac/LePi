/**
 * Sample app for streaming video over the local network (TCP)
 */

// LePi
#include <Common.h>
#include <LeptonCommon.h>
#include <LeptonCamera.h>

// Third party
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

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
    struct sockaddr_in remoteSocketInfo;
    bzero(&remoteSocketInfo, sizeof(sockaddr_in));  // Clear structure memory
    remoteSocketInfo.sin_family = AF_INET;
    remoteSocketInfo.sin_addr.s_addr = inet_addr(ipV4.c_str());
    remoteSocketInfo.sin_port = htons((u_short)portNumber);


    // Establish the connection with the server
    if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0)
    {
        cerr << "Client fail to connect to the server." << endl;
        cerr << "Error: " << strerror(errno) << endl;
        close(socketHandle);
        exit(EXIT_FAILURE);
    }


    // Send data
    constexpr int width{160};
    constexpr int height{120};
    int pckg_size = 10 + width * height * 2;// Header + IR img(80*60*2)
    char img[pckg_size];
    char msg[2]; // Header(MSG+Response=2)
    int rc = 0;
    cv::Mat ir_img (height, width, CV_8UC1);

    int count = 0;
    while (true)
    {

    	////////////////////////////////////////////////////////////////////////
    	///  Create Request
    	////////////////////////////////////////////////////////////////////////
        msg[0] = FRAME_REQUEST;
        msg[1] = FRAME_U8;


    	////////////////////////////////////////////////////////////////////////
    	///  Send Request
    	////////////////////////////////////////////////////////////////////////
        DPRINTF("[%d]CLIENT -- SEND -- Sending message... ", count);
        int sd = send(socketHandle, msg, sizeof(msg), 0);
        if (sd == -1)
        {
            cerr << "[" << count << "]CLIENT -- CONNECTION -- Lost." << endl;
            cerr << "Error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }
        DPRINTF(" Message sent! \n");


        ////////////////////////////////////////////////////////////////////////
        /// Receive response
        ////////////////////////////////////////////////////////////////////////
        //memset(img, 0, sizeof(img));
        int data_size = 0;
        do
        { // wait for data to be available
        	ioctl(socketHandle, FIONREAD, &data_size);
        } while (data_size < pckg_size);
        rc = recv(socketHandle, img, sizeof(img), 0);
        DPRINTF("[%d]CLIENT -- RECV -- Number of bytes read: %d \n", count, rc);

        // Check if connection is still open
        if (rc == -1)
        {
            cerr << "[" << count << "]CLIENT -- CONNECTION -- Lost." << endl;
            cerr << "Error: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        }

        // Check response header
        if (img[0] == FRAME_REQUEST)
        {
        	DPRINTF("[%d]CLIENT -- RECV -- FRAME_REQUEST response. \n", count);
        	memcpy(ir_img.data, img+10, width * height);
        }
        else if (img[0] == I2C_CMD)
        {
        	DPRINTF("[%d]CLIENT -- RECV -- I2C_CMD response. \n", count);
        }
        else if (img[0] == UNKNOWN_MSG)
        {
            cerr << "[" << count << "]CLIENT -- Server did not recognize your request." << endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            cerr << "[" << count << "]CLIENT -- Unable to decode server message." << endl;
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
