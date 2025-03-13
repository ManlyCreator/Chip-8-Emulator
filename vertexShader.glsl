#version 330 core

uniform mat4 projection;

layout (location = 0) in vec2 pos;

void main() {
  gl_Position = projection * vec4(pos, 0.0f, 1.0f);
}
