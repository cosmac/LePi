# Player of thermal stream using serial interface
message("-- Generating build for Player. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(Player)

# Player of thermal stream using parallel interface
message("-- Generating build for PlayerThread. Host: ${CMAKE_SYSTEM_NAME}")
add_subdirectory(PlayerThread)
