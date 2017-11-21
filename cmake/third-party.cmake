
# Load 3rd party libraries
function (third_party)

	# Load OpenCV
	find_package(OpenCV)
	set(OpenCV_FOUND ${OpenCV_FOUND} CACHE INTERNAL "OpenCV: Library found" FORCE)		
	if(OpenCV_FOUND)
		set(PKG_LIBRARIES ${OpenCV_LIBRARIES} ${OpenCV_LIBRARY} ${OpenCV_LIBS})
		set(PKG_INCLUDES ${OpenCV_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIR})

		set(OpenCV_LIBRARIES  ${PKG_LIBRARIES}
			CACHE INTERNAL "OpenCV: Project Library" FORCE)
		set(OpenCV_INCLUDE_DIRS ${PKG_INCLUDES}
			CACHE INTERNAL "OpenCV: Include Directories" FORCE)
		message(STATUS "OpenCV found")
	else()
		message(STATUS "OpenCV not found")
	endif()

	# Load bcm2835
	find_package(bcm2835 REQUIRED)
	set(bcm2835_FOUND ${bcm2835_FOUND} CACHE INTERNAL "bcm2835: Library found" FORCE)		
	if(bcm2835_FOUND)
		set(PKG_LIBRARIES ${bcm2835_LIBRARIES} ${bcm2835_LIBRARY} ${bcm2835_LIBS})
		set(PKG_INCLUDES ${bcm2835_INCLUDE_DIRS} ${bcm2835_INCLUDE_DIR})

		set(bcm2835_LIBRARIES  ${PKG_LIBRARIES}
			CACHE INTERNAL "bcm2835: Project Library" FORCE)
		set(bcm2835_INCLUDE_DIRS ${PKG_INCLUDES}
			CACHE INTERNAL "bcm2835: Include Directories" FORCE)
		message(STATUS "bcm2835 found")
	else()
		message(STATUS "bcm2835 not found")
	endif()
	
	# Load Threads
	find_package(Threads REQUIRED)
	set(Threads_FOUND ${Threads_FOUND} CACHE INTERNAL "Threads: Library found" FORCE)		
	if(Threads_FOUND)
		set(PKG_LIBRARIES ${Threads_LIBRARIES} ${Threads_LIBRARY} ${Threads_LIBS})
		set(PKG_INCLUDES ${Threads_INCLUDE_DIRS} ${Threads_INCLUDE_DIR})

		set(Threads_LIBRARIES  ${PKG_LIBRARIES}
			CACHE INTERNAL "Threads: Project Library" FORCE)
		set(Threads_INCLUDE_DIRS ${PKG_INCLUDES}
			CACHE INTERNAL "Threads: Include Directories" FORCE)
		message(STATUS "Threads found")
	else()
		message(STATUS "Threads not found")
	endif()

endfunction(third_party)