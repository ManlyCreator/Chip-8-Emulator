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

#define WIDTH 800
#define HEIGHT 600

// TODO: Implement point rendering within Chip8 display bounds
// TODO: Implement 0xDxyn, the draw command

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

  chip8.memory[0x200] = 0xC0;
  chip8.memory[0x201] = 0x0F;
  chipEmulateCycle(&chip8);
  
  // OpenGL Data
  float points[] = { 
     0.0f, 0.0f
  };
  GLuint VAO, VBO;
  GLuint shader;
  mat4 projection;

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
    glm_mat4_identity(projection);
    glm_ortho(-5.0f, WIDTH, HEIGHT, -5.0f, -1.0f, 1.0f, projection);

    shaderSetMatrix4(shader, "projection", projection);

    shaderUse(shader);
    glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, 1);
    // Poll Events & Swap Buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  return 0;
}
