# Player of thermal stream using serial interface
message("-- Generating build for Player. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(Player)

# Player of thermal stream using parallel interface
message("-- Generating build for PlayerThread. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(PlayerThread)

# TCP/IP Server for thermal stream using parallel interface
# and socket communication with a client app
message("-- Generating build for Server. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(Server)
