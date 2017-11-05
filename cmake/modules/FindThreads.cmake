# - Try to find Threads
# Once done this will define
# Threads_FOUND - System has Threads
# Threads_INCLUDE_DIRS - The Threads include directories
# Threads_LIBRARIES - The libraries needed to use bcm2835

find_path ( Threads_INCLUDE_DIR pthread.h )
find_library ( Threads_LIBRARY NAMES pthread )

set ( Threads_LIBRARIES ${Threads_LIBRARY} )
set ( Threads_INCLUDE_DIRS ${Threads_INCLUDE_DIR} )

include ( FindPackageHandleStandardArgs )
# handle the QUIETLY and REQUIRED arguments and set Threads_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args ( Threads DEFAULT_MSG Threads_LIBRARY Threads_INCLUDE_DIR )
