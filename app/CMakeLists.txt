# If OpenCV found --> activate player apps
if(OpenCV_FOUND)
    # Player of thermal stream using serial interface
    message("-- Generating build for Player. Host: ${CMAKE_SYSTEM_NAME}")
    add_subdirectory(Player)

    # Player of thermal stream using parallel interface
    message("-- Generating build for PlayerThread. Host: ${CMAKE_SYSTEM_NAME}")
    add_subdirectory(PlayerThread)
endif()

# TCP/IP Server for thermal stream using leptonAPI parallel interface
# and socket communication with a Client app
message("-- Generating build for Lepton Server. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(LePiServer)

if(OpenCV_FOUND)
    # TCP/IP Client for thermal stream using socket communication
    # with an existing Server app
    message("-- Generating build for Lepton Client. Host: ${CMAKE_SYSTEM_NAME}")
    add_subdirectory(LePiClient)
endif()