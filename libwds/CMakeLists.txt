set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall")

add_library(wds SHARED $<TARGET_OBJECTS:wdsrtsp>
                       $<TARGET_OBJECTS:wdscommon>
                       $<TARGET_OBJECTS:wdssink>
                       $<TARGET_OBJECTS:wdssource>)

include_directories ("${PROJECT_SOURCE_DIR}" )
add_executable(rtsp/rtsp-test rtsp/tests.cpp $<TARGET_OBJECTS:wdsrtsp> $<TARGET_OBJECTS:wdscommon>)
set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-whole-archive")

add_test(RTSPTest rtsp/rtsp-test)
