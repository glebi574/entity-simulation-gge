#version 330 core
in vec3 vPos;
in vec4 vCol;
uniform mat4 tm;
uniform mat4 pm;
out vec4 color;
void main() {
  gl_Position = pm * tm * vec4(vPos, 1.0);
  color = vCol;
}