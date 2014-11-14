#FIXME in the future: with cmake 2.8.12 and up it's better
#to use target_compile_options or add_compile_options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread -std=c++11")

include_directories ("${PROJECT_SOURCE_DIR}/include" )
include_directories ("${PROJECT_SOURCE_DIR}/wfd_parser" )

find_package(PkgConfig REQUIRED)
pkg_check_modules (GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})

pkg_check_modules (GST REQUIRED gstreamer-1.0)
include_directories(${GST_INCLUDE_DIRS})

add_library(mirac STATIC mirac-network.cpp mirac-gst-sink.cpp mirac-gst-test-source.cpp mirac-broker.cpp)

add_executable(network-test network-test.cpp)
target_link_libraries (network-test ${GLIB2_LIBRARIES} mirac)

add_executable(gst-test gst-test.cpp)
target_link_libraries (gst-test ${GLIB2_LIBRARIES} ${GST_LIBRARIES} mirac)
