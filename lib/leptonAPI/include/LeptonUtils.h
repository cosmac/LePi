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
#include <LEPTON_Types.h>

// C/C++
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


//------------------------------- SPI ----------------------------------------//
// SPI Config
extern int spi_fd;

/**
 * @brief Open SPI communication
 * @param spi_device  SPI device id
 * @param spi_speed   Desired SPI speed
 * @throw Runtime error if port can't be opened or configured
 */
void leptonSPI_OpenPort(int spi_device, uint32_t spi_speed);

/**
 * @brief Close SPI communication
 * @param spi_device  SPI device id
 * @throw Runtime error if port can't be closed
 */
void leptonSPI_ClosePort(int spi_device);

//------------------------------- I2C ----------------------------------------//
// I2C config
constexpr LEP_UINT16 kI2CPortID{1};
constexpr LEP_UINT16 kI2CPortBaudRate{400};
constexpr LEP_CAMERA_PORT_E kI2CPortType{LEP_CCI_TWI};

/**
 * @brief Open I2C communication with Lepton sensor
 * @throw Runtime error if communication can't be established
 */
void leptonI2C_connect();

/**
 * @brief Close I2C communication with Lepton sensor
 * @throw Runtime error if communication can't be closed
 */
void leptonI2C_disconnect();

/**
 * @brief Open shutter
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_ShutterOpen();

/**
 * @brief Run FFC
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_FFC();

/**
 * @brief Close shutter
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_ShutterClose();

/**
 * @brief Reboot lepton sensor
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_Reboot();

/**
 * @brief Set shutter mode to manual
 * @return Return true if operation succeed, false otherwise
 */
bool leptonI2C_ShutterManual();

/**
 * @brief Get thermal sensor core temperature
 * @return Return sensor temperature in Kelvins
 */
unsigned int leptonI2C_InternalTemp();

/**
 * @brief Get thermal sensor number/version
 * @return Return senors number/version
 */
unsigned int leptonI2C_SensorNumber();
