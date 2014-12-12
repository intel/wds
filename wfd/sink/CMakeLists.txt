set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c++11 -ggdb3 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c99 -ggdb3 -Wall")

include_directories ("${PROJECT_SOURCE_DIR}" )

add_library(sink STATIC
    sink.cpp cap_negotiation_state.cpp init_state.cpp
    wfd_session_state.cpp streaming_state.cpp
)