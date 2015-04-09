set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall")


add_library(wds SHARED $<TARGET_OBJECTS:wdsparser>
                       $<TARGET_OBJECTS:wdscommon>
                       $<TARGET_OBJECTS:wdssink>
                       $<TARGET_OBJECTS:wdssource>)
