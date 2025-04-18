#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;

void main() {
  gl_Position = vec4(pos, 0.0f, 1.0f);
  texCoord = inTexCoord;
}
