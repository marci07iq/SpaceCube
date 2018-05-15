#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texture;
layout(location = 2) in vec4 vertex_colour;
layout(location = 3) in float vertex_ripple;

uniform mat4 transform;
uniform float frame_time;

out vec4 colour;
out vec2 texture_c;

void main() {
  colour = vertex_colour;
  texture_c = vertex_texture;
  gl_Position = transform * (vec4(vertex_position, 1.0) + vertex_ripple * vec4(sin(vertex_ripple * 1000 * frame_time), sin(vertex_ripple * 1047.19755 * frame_time), sin(vertex_ripple * 998.268397 * frame_time), 0.0));
}