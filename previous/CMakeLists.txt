cmake_minimum_required(VERSION 3.10)
project(Chip8_Emulator VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# find_package(SDL2 REQUIRED)
# include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Chip8_Emulator src/main.cpp src/chip8.cpp)
# target_link_libraries(Chip8_Emulator ${SDL2_LIBRARIES})

target_include_directories(Chip8_Emulator PUBLIC
    "${PROJECT_BINARY_DIR}"
)


# cmake_minimum_required(VERSION 3.10)
# project(Tutorial VERSION 1.0)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# option(USE_MYMATH "Use tutorial math implementation" ON)

# configure_file(TutorialConfig.h.in TutorialConfig.h)

# if (USE_MYMATH)
#     add_subdirectory(math)
#     list(APPEND EXTRA_LIBS testroot)
#     list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/math")
    

# add_executable(Tutorial tutorial.cpp)

# target_link_libraries(Tutorial PUBLIC ${EXTRA_LIBS})

# # add_library(testroot root.cpp)

# target_include_directories(Tutorial PUBLIC
#     "${PROJECT_BINARY_DIR}"
#     ${EXTRA_INCLUDES}
#     # "${PROJECT_SOURCE_DIR}/math"
# )