cmake_minimum_required(VERSION 3.5)

project(Chip8Emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME} main.c)

include_directories(include/)

add_library(Chip8 src/chip8.c)
add_library(Shader src/shader.c)
add_library(Screen src/screen.c)
add_library(Buzzer src/buzzer.c)

target_link_libraries(Screen glad glfw3 GL m)
target_link_libraries(Buzzer openal m)
target_link_libraries(${PROJECT_NAME} Chip8 Shader Screen Buzzer)
