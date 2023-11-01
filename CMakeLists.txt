cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_BUILD_TYPE Release)

project("LuauWebsocket")

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "third-party/*.cpp" "third-party/*.c" "third-party/*.mm")

add_executable(LuauWebsocket 
    main.cpp
    websocket/websocket.cpp
    ${SOURCES}
)

target_include_directories(LuauWebsocket PRIVATE
    websocket
    third-party
    third-party/Luau/Ast/include
    third-party/Luau/Common/include
    third-party/Luau/Compiler/include
    third-party/Luau/VM/include
)