set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fvisibility=hidden -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall")

include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/libwds/rtsp/gen")

OPTION(WDS_OSSFUZZ "Enable oss-fuzz build." OFF)
IF(WDS_OSSFUZZ)
    add_executable(libwds_rtsp_fuzzer libwds_rtsp_fuzzer.cpp $<TARGET_OBJECTS:wdsrtsp> $<TARGET_OBJECTS:wdscommon>)
    target_link_libraries(libwds_rtsp_fuzzer PRIVATE $ENV{LIB_FUZZING_ENGINE})
ENDIF(WDS_OSSFUZZ)
