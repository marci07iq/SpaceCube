#version 420

in vec4 colour;
in vec2 texture_c;
uniform sampler2D myTexture;
out vec4 frag_colour;

void main() {
  frag_colour = (1-colour.a) * texture(myTexture, texture_c) + colour.a * colour;
}