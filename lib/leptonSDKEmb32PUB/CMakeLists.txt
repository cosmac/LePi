cmake_minimum_required(VERSION 3.0)

# Project name
project(leptonSDK)

# General flags
add_definitions(-DRELEASE)

# Dependencies
set(DEPENDENCIES )
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
list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Library Sources and Headers
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.h)
set( ${PROJECT_NAME}_TARGET_SRCS ${SOURCES} ${HEADERS})

# Create library
add_library (${PROJECT_NAME} ${${PROJECT_NAME}_TARGET_SRCS})
include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARIES})

# Expose library
set(${PROJECT_NAME}_LIBRARY ${PROJECT_NAME}
	CACHE INTERNAL "${PROJECT_NAME}: Project Library" FORCE)
set(${PROJECT_NAME}_LIBRARIES ${${PROJECT_NAME}_LIBRARIES}
	CACHE INTERNAL "${PROJECT_NAME}: Project Libraries" FORCE)
set(${PROJECT_NAME}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
	CACHE INTERNAL "${PROJECT_NAME}: Include Directory" FORCE)
set(${PROJECT_NAME}_INCLUDE_DIRS ${${PROJECT_NAME}_INCLUDE_DIRS}
	CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
set (${PROJECT_NAME}_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
	CACHE INTERNAL "${PROJECT_NAME}: Source Directory" FORCE)