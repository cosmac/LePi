# - Try to find bcm2835
# Once done this will define
# bcm2835_FOUND - System has bcm2835
# bcm2835_INCLUDE_DIRS - The bcm2835 include directories
# bcm2835_LIBRARIES - The libraries needed to use bcm2835
# bcm2835_DEFINITIONS - Compiler switches required for using bcm2835

find_path ( bcm2835_INCLUDE_DIR bcm2835.h )
find_library ( bcm2835_LIBRARY NAMES bcm2835 )

set ( bcm2835_LIBRARIES ${bcm2835_LIBRARY} )
set ( bcm2835_INCLUDE_DIRS ${bcm2835_INCLUDE_DIR} )

include ( FindPackageHandleStandardArgs )
# handle the QUIETLY and REQUIRED arguments and set bcm2835_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args ( bcm2835 DEFAULT_MSG bcm2835_LIBRARY bcm2835_INCLUDE_DIR )
