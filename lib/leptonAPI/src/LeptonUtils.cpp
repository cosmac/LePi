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
#include <LeptonAPI.h>
#include <LeptonUtils.h>
#include <LEPTON_OEM.h>
#include <LEPTON_SDK.h>
#include <LEPTON_SYS.h>
#include <LEPTON_Types.h>

// C/C++
#include <stdio.h>
#include <inttypes.h>


//============================================================================
// Lepton I2C Commands
//============================================================================

bool _connected{false};
LEP_CAMERA_PORT_DESC_T _port;

// Open Lepton I2C
void leptonI2C_connect() {
    LEP_RESULT result = LEP_OpenPort(kI2CPortID, kI2CPortType, kI2CPortBaudRate, &_port);
    if (result == LEP_OK) {
        std::cout << "Open I2C port: " <<_port.portID
                  << ", with address " << static_cast<int>(_port.deviceAddress)
                  << std::endl;
    }
    else {
        std::cerr << "Unable to open I2C communication.";
        throw std::runtime_error("I2C connection failed");
    }
    _connected = true;
}

// Close Lepton I2C
void leptonI2C_disconnect() {
	LEP_RESULT result = LEP_ClosePort(&_port);
    if (result == LEP_OK) {
        std::cout << "Close I2C port: " <<_port.portID
                  << ", with address " << static_cast<int>(_port.deviceAddress)
                  << std::endl;
    }
    else {
        std::cerr << "Unable to close I2C communication.";
        throw std::runtime_error("I2C close connection failed");
    }
    _connected = false;
}

// Set camera shutter mode to manual
bool leptonI2C_ShutterManual() {
    if (_connected) {
        // Get FFC-shutter mode
        LEP_SYS_FFC_SHUTTER_MODE_OBJ_T mode;
        LEP_RESULT res = LEP_GetSysFfcShutterModeObj(&_port, &mode);
        if (res == LEP_OK) {

            std::cout << "shutter mode " << mode.shutterMode << std::endl;
            // Set mode to manual
            mode.shutterMode = LEP_SYS_FFC_SHUTTER_MODE_MANUAL;
            res = LEP_SetSysFfcShutterModeObj(&_port, mode);
            if (res == LEP_OK) {
                // Check mode
                res = LEP_GetSysFfcShutterModeObj(&_port, &mode);
                std::cout << "shutter mode " << mode.shutterMode << std::endl;
            }
        }
        return res == LEP_OK;
    }
    return false;
}

// Close/Open camera shutter
bool leptonI2C_ShutterOpen() {
    if (_connected) {
        LEP_SYS_SHUTTER_POSITION_E position = LEP_SYS_SHUTTER_POSITION_OPEN;
        return LEP_SetSysShutterPosition(&_port, position) == LEP_OK;
    }
    return false;
} 
bool leptonI2C_ShutterClose() {
    if (_connected) {
        LEP_SYS_SHUTTER_POSITION_E position = LEP_SYS_SHUTTER_POSITION_CLOSED;
        return LEP_SetSysShutterPosition(&_port, position) == LEP_OK;
    }
    return false;
} 

// Perform FFC
bool leptonI2C_FFC() {
    if (_connected) {
        return LEP_RunSysFFCNormalization(&_port) == LEP_OK;
    }
    return false;
}

// Reboot sensor
bool leptonI2C_Reboot() {
    if (_connected) {
        std::cout << "Reboot lepton sensor..." << std::endl;
        return LEP_RunOemReboot(&_port) == LEP_OK;
    }
    return false;
}

// Get internal temperature
unsigned int leptonI2C_InternalTemp() {

    LEP_SYS_FPA_TEMPERATURE_KELVIN_T sensor_temp_kelvin{0};
    if (_connected) {
        LEP_GetSysFpaTemperatureKelvin(&_port, &sensor_temp_kelvin);
    }

    return static_cast<unsigned int>(sensor_temp_kelvin);
}

// Get lepton type
unsigned int leptonI2C_SensorNumber() {

    //LEP_SYS_FLIR_SERIAL_NUMBER_T sysSerialNumberBuf;
    //LEP_GetSysFlirSerialNumber(&_port, &sysSerialNumberBuf);
    LEP_SYS_VIDEO_ROI_T sceneRoi;
    LEP_GetSysSceneRoi(&_port, &sceneRoi);
    if (sceneRoi.endCol == 79 && sceneRoi.endRow == 59) {
        return 2;
    }
    else if (sceneRoi.endCol == 159 && sceneRoi.endRow == 119) {
        return 3;
    }                        
    return 0;
}

//============================================================================
// Lepton SPI Communication
//============================================================================

// SPI config
int spi_fd{-1};
unsigned char spi_mode{SPI_MODE_3};
unsigned char spi_bits_per_word{8};

// Open SPI port
void leptonSPI_OpenPort (int spi_device, uint32_t spi_speed)
{
    int status_value{-1};

    // Select SPI device and open communication
    if (spi_device) {
        spi_fd = open(std::string("/dev/spidev0.1").c_str(), O_RDWR);
    }
    else {
        spi_fd = open(std::string("/dev/spidev0.0").c_str(), O_RDWR);
    }

    if (spi_fd < 0) {
        std::cerr << "Error - Could not open SPI device" << std::endl;
        throw std::runtime_error("Connection failed.");
    }

    // Set SPI mode WR
    //SPI_MODE_0 (0,0)  CPOL=0 (Clock Idle low level), CPHA=0 (SDO transmit/change edge active to idle)
    //SPI_MODE_1 (0,1)  CPOL=0 (Clock Idle low level), CPHA=1 (SDO transmit/change edge idle to active)
    //SPI_MODE_2 (1,0)  CPOL=1 (Clock Idle high level), CPHA=0 (SDO transmit/change edge active to idle)
    //SPI_MODE_3 (1,1)  CPOL=1 (Clock Idle high level), CPHA=1 (SDO transmit/change edge idle to active)
    status_value = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
    if(status_value < 0) {
        std::cerr << "Could not set SPIMode (WR)...ioctl fail" << std::endl;
        throw std::runtime_error("SPI config failed.");
    }

    // Set SPI Mode RD
    status_value = ioctl(spi_fd, SPI_IOC_RD_MODE, &spi_mode);
    if(status_value < 0) {
        std::cerr << "Could not set SPIMode (RD)...ioctl fail" << std::endl;
        throw std::runtime_error("SPI config failed.");
    }

    // Set SPI bits per word WR
    status_value = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits_per_word);
    if(status_value < 0) {
        std::cerr << "Could not set SPI bitsPerWord (WR)...ioctl fail" << std::endl;
        throw std::runtime_error("SPI config failed.");
    }

    // Set SPI bits per word Rd
    status_value = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits_per_word);
    if(status_value < 0) {
        std::cerr << "Could not set SPI bitsPerWord(RD)...ioctl fail" << std::endl;
        throw std::runtime_error("SPI config failed.");
    }

    // Set SPI bus speed WR
    status_value = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0) {
        std::cerr << "Could not set SPI speed (WR)...ioctl fail" << std::endl;
        throw std::runtime_error("SPI config failed.");
    }

    // Set SPI bus speed RD
    status_value = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0) {
        std::cerr << "Could not set SPI speed (RD)...ioctl fail" << std::endl;
        throw std::runtime_error("SPI config failed.");
    }

    std::cout << "Open SPI port: " << spi_device
              << ", with address " << spi_fd
              << std::endl;
}


// Close SPI connection
void leptonSPI_ClosePort(int spi_device)
{
    int status_value{-1};

    // Close connection
    status_value = close(spi_fd);
    if(status_value < 0)  {
        std::cerr << "Error - Could not close SPI device" << std::endl;
        throw std::runtime_error("Closing connection failed.");
    }

    std::cout << "Close SPI port: " << spi_device
              << ", with address " << spi_fd
              << std::endl;
}
