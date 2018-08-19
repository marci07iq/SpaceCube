#version 420

in vec2 texture_c;
uniform sampler2D myTexture;
out vec4 frag_colour;

void main() {
  frag_colour = texture(myTexture, texture_c);
  if (frag_colour.a < 0.01) {
	discard; // yes: discard this fragment
  }
}