#ifndef SCREEN_H
#define SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

#define WIDTH 1920
#define HEIGHT 960

typedef struct {
  GLFWwindow *window; 
  Shader shader;
  GLuint texture;
  GLuint VAO;
} Screen;

Screen screenInit(const char *vsPath, const char *fsPath, unsigned texWidth, unsigned texHeight, unsigned char *texData);

#endif
