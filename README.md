# LePi [lee-pahy]
*Copyright (c) 2017 Andrei Claudiu Cosma*

LePi is a lightweight library for FLIR Lepton (2, 2.5 and 3) and Raspberry Pi. The Lepton version will be automatically detected at run time. The SPI port can be specified in the app, by default the LePi apps are using SPI0. 

Please make sure that the sensor is properly wired. Here is an example for Rapberry Pi2:
- SCL: Pin#5
- SDA: Pin#3
- VIN: Pin#1
- GND: Pin#25
- CLK: Pin#23
- MISO: Pin#21
- MOSI: Pin#19
- CS: Pin#24


## Dependecies
- [REQUIRED] CMake:
```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential cmake pkg-config
```
- [REQUIRED] BCM2835: http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz
```
tar zxvf bcm2835-1.52.tar.gz
cd bcm2835-1.52
./configure
make
sudo make check
sudo make install
```
- [REQUIRED] OpenCV: https://github.com/opencv/opencv/releases/tag/3.3.0

You can follow these steps for installation https://www.pyimagesearch.com/2016/04/18/install-guide-raspberry-pi-3-raspbian-jessie-opencv-3/, or other tutorial that you find useful and easy to follow. I'm using OpenCV 3.3.0, but any version after 3.0.0 should be okay.

**_Note:_** The library can be built without OpenCV too. In that case, the demo apps will not be available. To do that, open the CMake file and comment out the following line `add_subdirectory(app)`. I'm planning on writing an example that uses the librarry without OpenCV.

## Build
LePi build is based on the CMake portable pre-build tool. Follow the next steps to build the library and the apps:
```
git clone https://github.com/cosmac/LePi.git
cd LePi
mkdir build
cd build
cmake ..
make
```

## Run
LePi library comes with a set of integrated demo apps, that shows how to use the Lepton camera serial and parallel interface.
Once the library is build, demo apps can be found in the install/bin folder. To run a demo app:
```
cd LePi/install/bin
sudo ./Player
```
**_Note1:_** `sudo` is always required when you run an app communicating with the Lepton sensor

**_Note2:_** You may observe different frame rates between Letpon 2 and 3. That is because the library reads all the frames available on the SPI port! Based on the Lepton 2 datasheet you can observe that since the real frame rate is  ~9 fps, they send the same frame 3 times until the next frame is available. Now, for Lepton 3, they decided to send discard packets until a new frame is avialble. For this reason you may see a frame rate of ~26 fps for Lepton 2 and ~9 fps for Lepton 3. Anyway, in both cases, there are only ~9 unique frames per second.

## Other resources
- Breakout board pin layout: http://www.pureengineering.com/projects/lepton
- FLIR Lepton datasheet and sdk: http://www.flir.com/cores/display/?id=51878
- BCM2835: http://www.airspayce.com/mikem/bcm2835/index.html
- OpenCV: https://github.com/opencv/opencv
- Raspberyy Pi pinout: https://pinout.xyz/

