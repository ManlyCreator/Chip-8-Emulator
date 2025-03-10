// Standard Libraries
#include <stdio.h>
#include <stdlib.h>

// OpenGL Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Chip8
#include "chip8.h"

#define WIDTH 800
#define HEIGHT 600

// TODO: Debug switch-unreachable warning
// TODO: Start decoding instructions 0x9000 onwards

int main(int argc, char **argv) {
  Chip8 chip8 = chipInitialize();
  GLFWwindow *window;

  // GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Chip8", NULL, NULL);

  if (!window) {
    printf("Window creation failed\n");
    return -1;
  }

  // GLAD
  if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to load GLAD\n");
    return -1;
  }

  // Chip8
  if (!chipLoadROM(&chip8, "../roms/connect4.ch8"))
    return -1;

  chipEmulateCycle(&chip8);

  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    // Poll Events & Swap Buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  return 0;
}
