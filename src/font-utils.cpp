#include "g-utils.h"

int FontHandler::init() {
  if (FT_Init_FreeType(&ft)) {
    std::cerr << "Failed to initialize FreeType" << std::endl;
    return 1;
  }
  if (FT_New_Face(ft, "FiraCode-Regular.ttf", 0, &face)) {
    std::cerr << "Failed to load font" << std::endl;
    return 1;
  }
  FT_Set_Pixel_Sizes(face, 0, 48);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (uint8_t c = 0; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cerr << "Failed to load glyph" << std::endl;
      continue;
    }
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Character ch = {
      texture,
      face->glyph->advance.x,
      face->glyph->bitmap.width, face->glyph->bitmap.rows,
      face->glyph->bitmap_left, face->glyph->bitmap_top
    };
    characters.emplace_back(ch);
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  return 0;
}