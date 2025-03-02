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

void TextVO::gen_vertexes(Character& ch, uint32_t offset) {
  float
    x = ch.bearing_x + offset,
    y = ch.bearing_y - ch.size_y,
    w = ch.size_x,
    h = ch.size_y;
  float pos[4][3] = {
    {x, y},
    {x + w, y},
    {x, y + h},
    {x + w, y + h}
  };
  vertexes.emplace_back(Glyph(pos[2], 0));
  vertexes.emplace_back(Glyph(pos[0], 1));
  vertexes.emplace_back(Glyph(pos[1], 3));
  vertexes.emplace_back(Glyph(pos[2], 0));
  vertexes.emplace_back(Glyph(pos[1], 3));
  vertexes.emplace_back(Glyph(pos[3], 2));
}

Glyph::Glyph(float pos[3], uint8_t corner) {
  for (int i = 0; i < 3; ++i)
    this->pos[i] = pos[i];
  quad[0] = corner > 1; // 0 0 1 1
  quad[1] = corner % 2; // 0 1 0 1
}

FontHandler GWindow::font_handler;
TriangleVOData GWindow::tvo_data;
TextVOData GWindow::gvo_data;