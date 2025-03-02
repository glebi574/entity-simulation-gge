#include "g-utils.h"

uint32_t rgba_to_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return ((r * 256 + g) * 256 + b) * 256 + a;
}

void TriangleVO::gen_vertexes(std::vector<float>& positions, std::vector<uint32_t>& colors) {
  for (int i = 0; i < colors.size(); ++i) {
    float pos[3] = { positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2] };
    vertexes.emplace_back(Vertex(pos, colors[i]));
  }
}