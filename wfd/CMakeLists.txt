set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fvisibility=hidden")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall")


add_library(wysiwidi SHARED $<TARGET_OBJECTS:wfdparser>
                            $<TARGET_OBJECTS:wfdcommon>
                            $<TARGET_OBJECTS:wfdsink>
                            $<TARGET_OBJECTS:wfdsource>)