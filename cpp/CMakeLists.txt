# Alternative build for Visual Studio / CLion / VS Code (CMake downloads raylib for you).
cmake_minimum_required(VERSION 3.16)
project(FlappyBird CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(raylib)

add_executable(FlappyBird src/main.cpp src/Game.cpp src/Bird.cpp src/Pipe.cpp)
target_link_libraries(FlappyBird PRIVATE raylib)

if(WIN32)   # no black console window behind the game
    set_target_properties(FlappyBird PROPERTIES WIN32_EXECUTABLE TRUE)
    if(MSVC)
        target_link_options(FlappyBird PRIVATE /ENTRY:mainCRTStartup)
    endif()
endif()
