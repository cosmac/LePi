# Player of thermal stream using serial interface
message("-- Generating build for Player. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(Player)

# Player of thermal stream using parallel interface
message("-- Generating build for PlayerThread. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(PlayerThread)

# TCP/IP Publisher for thermal stream using leptonAPI parallel interface
# and socket communication with a Subscriber app
message("-- Generating build for Server. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(Publisher)

# TCP/IP Subscriber for thermal stream using socket communication
# with an existing Publisher app
message("-- Generating build for Server. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(Subscriber)