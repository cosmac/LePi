cmake_minimum_required (VERSION 3.0)

project(LePiServer)

# Dependencies
set(DEPENDENCIES leptonAPI utils)
list(LENGTH DEPENDENCIES num_dependencies)
if(num_dependencies)
	foreach(lib_name ${DEPENDENCIES})
		#message(STATUS "${${lib_name}_INCLUDE_DIRS}")
		#message(STATUS "${${lib_name}_LIBRARIES}")
        list(APPEND MY_INCLUDES ${${lib_name}_INCLUDE_DIR})
        list(APPEND MY_INCLUDES ${${lib_name}_INCLUDE_DIRS})
        list(APPEND MY_LIBRARIES ${${lib_name}_LIBRARY})
        list(APPEND MY_LIBRARIES ${${lib_name}_LIBRARIES})
    endforeach()
    list(LENGTH MY_INCLUDES includes)
	if (includes)
		list(REMOVE_DUPLICATES MY_INCLUDES)
	endif()
    list(LENGTH MY_LIBRARIES libraries)
	if (libraries)
		list(REMOVE_DUPLICATES MY_LIBRARIES)
	endif()
    set(${PROJECT_NAME}_INCLUDE_DIRS ${MY_INCLUDES})
    set(${PROJECT_NAME}_LIBRARIES ${MY_LIBRARIES})
endif()
list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS  ${PROJECT_SOURCE_DIR})

# Sources and Headers
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
set( ${PROJECT_NAME}_TARGET_SRCS ${SOURCES} ${HEADERS})

# Create app 
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_TARGET_SRCS})
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARIES})
