// Standard Libraries
#include <stdio.h>
#include <stdlib.h>

// OpenGL Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// External Libraries
#include "chip8.h"
#include "shader.h"

#define WIDTH 1920
#define HEIGHT 960

// TODO: Render Chip8 Logo
// TODO: Render Space Invaders start screen, may need to implement remaining instructions first

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

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
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  // GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to load GLAD\n");
    return -1;
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  // Chip8
  if (!chipLoadROM(&chip8, "../roms/IBM_Logo.ch8"))
    return -1;

  /*chip8.memory[0x200] = 0xA0;*/
  /*chip8.memory[0x201] = 0x4B;*/
  /*chip8.memory[0x202] = 0xDF;*/
  /*chip8.memory[0x203] = 0xF5;*/
  /*chipEmulateCycle(&chip8);*/
  /*chipEmulateCycle(&chip8);*/

  
  /* OpenGL Data */
  float plane[] = {
    // Vertices   // Texture Coordinates
    -1.0f,  1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 1.0f, 1.0f
  };
  GLuint VAO, VBO;
  GLuint shader;
  GLuint texture;

  // Shader
  if (!shaderConstruct(&shader, "../vertexShader.glsl", "../fragmentShader.glsl"))
    return -1;

  // Texture
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, chip8.display);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Vertex Array
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Vertex Buffer
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // Callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  chipEmulateCycle(&chip8);
  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    // Emulation Cycle
    chipEmulateCycle(&chip8);

    // Display Debugger
    /*for (int y = 0; y < DISPLAY_HEIGHT; y++) {*/
    /*  for (int x = 0; x < DISPLAY_WIDTH; x++) {*/
    /*    printf("%d", chip8.display[y * 64 + x]);*/
    /*  }*/
    /*  printf("\n");*/
    /*}*/
    
    // Clear Commands
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    // Draw Commands
    glBindVertexArray(VAO);
    shaderUse(shader);
    shaderSetInt(shader, "texSample", 0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, chip8.display);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Poll Events & Swap Buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}
