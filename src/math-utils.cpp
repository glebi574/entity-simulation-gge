#include "math-utils.h"

int irand(int l, int h) {
  return l + rand() % (h - l);
}

float
  __c_frand = static_cast<float>(RAND_MAX),
  __c_frandr = __c_frand / 2;

float frand() {
  return static_cast<float>(rand()) / __c_frand;
}

float frandr() {
  return static_cast<float>(rand()) / __c_frandr - 1.f;
}

float frand(float l, float h) {
  return l + static_cast<float>(rand()) / __c_frand * (h - l);
}

Vertex::Vertex(float pos[3], uint32_t color) {
  for (int i = 0; i < 3; ++i)
    this->pos[i] = pos[i];
  set_hex_color(color);
}

void ColorComponent::set_hex_color(uint32_t color) {
  for (int i = 3; i >= 0; --i) {
    this->color[i] = (color & 0xff) / 255.f;
    color >>= 8;
  }
}

void TMatrix::set_angle(float rx, float ry, float rz) {
  float sx = sin(rx), cx = cos(rx),
        sy = sin(ry), cy = cos(ry),
        sz = sin(rz), cz = cos(rz);

  m[0][0] = cy * cz;
  m[0][1] = sx * sy * cz - cx * sz;
  m[0][2] = cx * sy * cz + sx * sz;

  m[1][0] = cy * sz;
  m[1][1] = sx * sy * sz + cx * cz;
  m[1][2] = cx * sy * sz - sx * cz;

  m[2][0] = -sy;
  m[2][1] = sx * cy;
  m[2][2] = cx * cy;
}

void TMatrix::set_scale(float s) {
  m[0][0] *= s;
  m[1][1] *= s;
  m[2][2] *= s;
}

void TMatrix::set_scale(float sx, float sy, float sz) {
  m[0][0] *= sx;
  m[1][1] *= sy;
  m[2][2] *= sz;
}

void TMatrix::set_offset(float ox, float oy, float oz) {
  m[3][0] = ox;
  m[3][1] = oy;
  m[3][2] = oz;
}