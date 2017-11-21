# LePi - Demo Apps
*Copyright (c) 2017 Andrei Claudiu Cosma*

LePi Demo Apps are a great start point for understanding how the library works.

## Player
- LePi Player is a simple app that uses OpenCV and LePi serial interface to show the thermal stream from sensor.

![Player](https://github.com/cosmac/LePi/blob/master/resources/Player_Lepton3.png)

## PlayerThread
- LePi PlayerThread is a simple app that uses OpenCV and LePi parallel interface to show the thermal stream from sensor.

![PlayerThread](https://github.com/cosmac/LePi/blob/master/resources/PlayerThread_Lepton3.png)

## LePiServer
- LePi Server is a simple app that uses TCP/IP connection to stream the thermal sensor frames to a client app. 
- This is useful when the thermal frames consumer is a different app. 
- The client can run on the same machine with the Server (raspberry Pi) or on a different machine.

__Note:__ this implementation allows the user to define the Client app in a different language (e.g. Java, Python, or Javascript).

![Server](https://github.com/cosmac/LePi/blob/master/resources/LePiServer.png)

## LePiClient
- LePi Client is a simple app that uses TCP/IP connection to show the thermal sensor frames received from LePi Server app. 
- This is a C++ implementation, but as I mentioned above, you can use a different language for the client app.
- If the client app runs on a different machine, you should know the IP address of the reaspberry Pi, where the LePi Server runs.

![Client](https://github.com/cosmac/LePi/blob/master/resources/LePiClient.png)
