cmake_minimum_required(VERSION 3.5)

project(Chip8Emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME} main.c)

include_directories(include/)

add_library(Chip8 src/chip8.c)
add_library(Shader src/shader.c)
add_library(Screen src/screen.c)

find_package(OpenAL REQUIRED)

target_link_libraries(Screen glad glfw3 GL m)
target_link_libraries(${PROJECT_NAME} OpenAL::OpenAL Chip8 Shader Screen)
