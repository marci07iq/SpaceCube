#version 420

in vec2 texture_c;
//in vec4 tint_c;
in float light_c;
uniform sampler2D myTexture;
out vec4 frag_colour;

void main() {
  vec4 tex = texture(myTexture, texture_c);
  //frag_colour = vec4(mix(tex.rgb, tint_c.rgb, tint_c.a), tex.a);
  frag_colour = vec4(tex.rgb * light_c, tex.a);
  if (frag_colour.a < 0.01) {
	discard; // yes: discard this fragment
  }
}