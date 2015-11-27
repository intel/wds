cmake_minimum_required(VERSION 2.8)

project(wds CXX)

enable_testing()

include(GNUInstallDirs)

add_subdirectory(libwds/rtsp)
add_subdirectory(libwds/common)
add_subdirectory(libwds/source)
add_subdirectory(libwds/sink)
add_subdirectory(libwds)
add_subdirectory(p2p)
add_subdirectory(mirac_network)
add_subdirectory(sink)
add_subdirectory(desktop_source)
