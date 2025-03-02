#version 330 core

uniform sampler2D text;
uniform vec4 tCol;

in vec2 tPos;

out vec4 FragColor;

void main() {
  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tPos).r);
  FragColor = tCol * sampled;
}