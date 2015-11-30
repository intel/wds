#FIXME in the future: with cmake 2.8.12 and up it's better
#to use target_compile_options or add_compile_options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -fvisibility=hidden -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Wall")

# Make directory for generated parser files
set(PARSER_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gen")
file(MAKE_DIRECTORY ${PARSER_GEN_DIR})

include_directories ("${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" "${PARSER_GEN_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

find_package(BISON REQUIRED)
BISON_TARGET(Parser parser.ypp ${PARSER_GEN_DIR}/parser.tab.cpp)

add_custom_target(remove_exceptions
    COMMAND ./remove_exceptions.sh ${PARSER_GEN_DIR}/parser.tab.cpp
    DEPENDS ${BISON_Parser_OUTPUTS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Removing exceptions")

find_package(FLEX REQUIRED)
FLEX_TARGET(MessageLexer messagelexer.l  ${PARSER_GEN_DIR}/messagescanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(MessageLexer Parser)
FLEX_TARGET(HeaderLexer headerlexer.l  ${PARSER_GEN_DIR}/headerscanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(HeaderLexer Parser)
FLEX_TARGET(ErrorLexer errorlexer.l  ${PARSER_GEN_DIR}/errorscanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(ErrorLexer Parser)

add_library(wdsrtsp OBJECT
    ${BISON_Parser_OUTPUTS}
    ${FLEX_MessageLexer_OUTPUTS}
    ${FLEX_ErrorLexer_OUTPUTS}
    ${FLEX_HeaderLexer_OUTPUTS}
    driver.cpp message.cpp header.cpp transportheader.cpp payload.cpp
    options.cpp reply.cpp getparameter.cpp setparameter.cpp play.cpp
    pause.cpp teardown.cpp setup.cpp property.cpp genericproperty.cpp
    formats3d.cpp audiocodecs.cpp clientrtpports.cpp
    contentprotection.cpp coupledsink.cpp displayedid.cpp
    presentationurl.cpp route.cpp triggermethod.cpp
    videoformats.cpp i2c.cpp avformatchangetiming.cpp uibcsetting.cpp
    standbyresumecapability.cpp standby.cpp idrrequest.cpp connectortype.cpp
    preferreddisplaymode.cpp uibccapability.cpp propertyerrors.cpp scanner.cpp
)

add_dependencies( wdsrtsp remove_exceptions )

add_executable(test-wds tests.cpp $<TARGET_OBJECTS:wdsrtsp> $<TARGET_OBJECTS:wdscommon>)
set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-whole-archive")
target_link_libraries (test-wds)

add_test(WfdTest test-wds)
