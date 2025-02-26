#ifndef G_MATH_UTILS
#define G_MATH_UTILS

#define PI  3.14159265358979323846
#define TAU 6.28318530717958647693

#include <iostream>
#include <vector>
#include <random>

int irand(int l, int h);

float frand(float l, float h);

struct Vertex {
public:
  float pos[3] = {};
  float color[4] = {};

  Vertex(float pos[3], uint32_t color); // Defines a vertex with provided position and hex color

  void set_hex_color(uint32_t color); // Sets a color of the vertex with provided hex color
};

struct TMatrix {
public:
  float m[4][4] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  void set_angle(float rx, float ry, float rz);

  void set_scale(float sx, float sy, float sz);

  void set_offset(float ox, float oy, float oz);
};

#endif