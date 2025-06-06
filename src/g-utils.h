#ifndef G_UTILS
#define G_UTILS

#include "math-utils.h"

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

uint32_t rgba_to_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// Contains common data, like locations of stuff in shaders
class VObjectData {
public:
  GLint tm_location;
};

class TriangleVOData : public VObjectData {
public:
  GLint vpos_location, vcol_location;
};

class TextVOData : public VObjectData {
public:
  GLint vpos_location, vtex_location, tcol_location;
};

class VObject {
public:
  GLuint VAO, VBO;
  TMatrix tm;

  void clear();
};

// Contains data to draw triangles
class TriangleVO : public VObject {
public:
  std::vector<Vertex> vertexes;

  // Adds vertexes, based on provided coordinate and color tables
  void gen_vertexes(std::vector<float>& positions, std::vector<uint32_t>& colors);
};

// Stores glyph data
struct Character {
public:
  uint32_t texture_id, advance;
  int size_x, size_y, bearing_x, bearing_y;
};

struct Glyph {
public:
  float pos[3] = {};
  float quad[2] = {};

  Glyph(float pos[3], uint8_t corner);
};

// Contains data to draw text
class TextVO : public VObject, public ColorComponent {
public:
  std::string str;
  std::vector<Glyph> vertexes;

  void gen_vertexes(Character& ch, uint32_t offset);
};

// Handles font initialization and rendering
struct FontHandler {
public:
  FT_Library ft;
  FT_Face face;
  std::vector<Character> characters;

  int init();
};

// Used to create windows and works with them
class GWindow {
public:
  int width, height;
  double size_ratio = 1;
  GLFWwindow* window;
  GLuint pm_location, triangle_sp, glyph_sp;
  TMatrix pm;
  std::vector<std::unique_ptr<TriangleVO>> tvo;
  std::vector<std::unique_ptr<TextVO>> gvo;

  static FontHandler font_handler;
  static TriangleVOData tvo_data;
  static TextVOData gvo_data;

  void set_orthographic_projection(float width, float height);

  void new_shader_program(GLuint* sp, std::string path_vs, std::string path_fs);

  // Creates window and defines stuff
  int init();

  // Main loop of the program
  void window_proc();

  TriangleVO* add_tvo(std::vector<float>& positions, std::vector<uint32_t>& colors);

  TextVO* add_text(std::string text, float x, float y, float scale, uint32_t color);

  void get_cursor_pos(double& x, double& y);

  // Draws all triangles
  void draw_tvo();

  // Draws all glyphs
  void draw_gvo();

  void remove_vo(VObject* vo);

  // Free memory
  void clear();
};

#endif