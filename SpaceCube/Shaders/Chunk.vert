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
  float vertexPeriod = mod(vertex_ripple, 0.01);
  gl_Position = transform * (vec4(vertex_position, 1.0) + vertex_ripple * vec4(0.0, 0.0, sin(vertexPeriod * 480.832611 * frame_time), 0.0));//* vec4(sin(vertexPeriod * 500 * frame_time), sin(vertexPeriod * 523.598776 * frame_time), sin(vertexPeriod * 480.832611 * frame_time), 0.0));
}