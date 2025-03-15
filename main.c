// Standard Libraries
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stdio.h>
#include <stdlib.h>

// OpenGL Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

// External Libraries
#include "chip8.h"
#include "shader.h"

#define WIDTH 1920
#define HEIGHT 960

// TODO: Only render pixels that are set to 1 in chip8.display

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
  if (!chipLoadROM(&chip8, "../roms/spaceInvaders.ch8"))
    return -1;

  chip8.memory[0x000] = 0x3C;
  chip8.memory[0x001] = 0xC3;
  chip8.memory[0x002] = 0xFF;

  chip8.memory[0x200] = 0xA0;
  chip8.memory[0x201] = 0x00;
  chip8.memory[0x202] = 0xD0;
  chip8.memory[0x203] = 0x13;
  chipEmulateCycle(&chip8);
  chipEmulateCycle(&chip8);
  
  // OpenGL Data
  unsigned numPoints = DISPLAY_WIDTH * DISPLAY_HEIGHT * 2;
  float points[numPoints];
  /* USE THIS LOGIC TO DRAW PIXELS */
  for (int i = 0; i < numPoints; i += 2) {
    if (!chip8.display[i / 2]) continue;
    // TODO: Properly fill the screen with pixels
    float x =      (((i / 2)) % 64) + (15.0f + (30.0f * (int)((i / 2) % 64)));
    float y = (int)((i / 2) / 64) + (15.0f + (30.0f * (int)((i / 2) / 64)));
    printf("x = %.2f\n", x);
    printf("y = %.2f\n", y);
    points[i] = x;
    points[i + 1] = y;
  }
  GLuint VAO, VBO;
  GLuint shader;
  mat4 projection;
  glm_mat4_identity(projection);
  glm_ortho(0.0f, WIDTH, HEIGHT, 0.0f, -1.0f, 1.0f, projection);

  // Shader
  if (!shaderConstruct(&shader, "../vertexShader.glsl", "../fragmentShader.glsl"))
    return -1;

  // Vertex Array
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Vertex Buffer
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(points[0]), (void*)0);

  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    shaderSetMatrix4(shader, "projection", projection);

    shaderUse(shader);
    glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, (sizeof(points) / sizeof(points[0]) / 2));
    // Poll Events & Swap Buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  return 0;
}
