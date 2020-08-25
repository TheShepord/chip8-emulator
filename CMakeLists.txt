cmake_minimum_required(VERSION 3.10)
project(Chip8_Emulator VERSION 1.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(SDL2 REQUIRED)
# find_package(Qt5Widgets REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Chip8_Emulator src/main.cpp src/chip8.cpp src/clock.cpp src/sound.cpp)
target_link_libraries(Chip8_Emulator ${SDL2_LIBRARIES}) #Qt5::Widgets)

target_include_directories(Chip8_Emulator PUBLIC
    "${PROJECT_BINARY_DIR}"
)