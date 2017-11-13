
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
    const uint32_t bpp{2};
    const uint32_t header_size{2 + 4 + 4}; // Header(MSG(1) + CMD(1) + To(4) + SensorTemp(4)
    std::vector<uint8_t> imgU8(width * height);
    std::vector<uint16_t> imgU16(width * height);
    std::vector<uint8_t> frame(header_size + width * height * bpp); // Header + IR img
    char msg[2]; // Header(MSG+Response=2)
    int rc = 0;
    memset(msg, 1, sizeof(msg));

    int count = 0;
    while (true) {
    
        //  Receive Request
        rc = recv(socketConnection, msg, sizeof(msg), 0);
        DPRINTF("[%d]SERVER -- RECV -- Number of bytes read: %d \n", count, rc);

         // Check if connection is still open
         if (rc == -1) {
             std::cerr << "[" << count << "]SERVER -- CONNECTION -- Lost." << std::endl;
             std::cerr << "Error: " << strerror(errno) << std::endl;
             exit(EXIT_FAILURE);
         }

         // Frame request msg
         if (msg[0] == FRAME_REQUEST) {
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

        // Send response
        DPRINTF("[%d]SERVER -- SEND -- Sending message response... ", count);
        int sd = send(socketConnection, frame.data(), frame.size(), 0);
        if (sd == -1) {
            std::cerr << "[" << count << "]SERVER -- CONNECTION -- Lost." << std::endl;
            std::cerr << "Error: " << strerror(errno) << std::endl;
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
