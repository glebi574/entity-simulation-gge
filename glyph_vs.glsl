#version 330 core

uniform mat4 pm;
uniform mat4 tm;

in vec3 vPos;
in vec2 vTex;

out vec2 tPos;

void main() {
  gl_Position = pm * tm * vec4(vPos, 1.0);
  tPos = vTex;
}