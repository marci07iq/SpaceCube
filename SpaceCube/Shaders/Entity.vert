#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;

uniform mat4 transform;

out vec2 texture_c;

void main() {
  texture_c = vertex_texture;
  gl_Position = transform * vec4(vertex_position, 1.0);
}