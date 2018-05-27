#version 420

in vec4 colour;
in vec2 texture_c;
uniform sampler2D myTexture;

void main() {
  vec4 texCol = texture(myTexture, texture_c);
  gl_FragColor = texCol.a * vec4(texCol.rgb, 1.0) + (1-texCol.a) * colour;
  if (gl_FragColor.a < 0.01) {
	discard; // yes: discard this fragment
  }
}