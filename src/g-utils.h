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

// Vertex Object - stores VAO, VBO and other stuff for each set of vertexes
struct VObject {
public:
  GLuint VAO, VBO;
  GLint vpos_location, vcol_location, tm_location;
  TMatrix tm;
  std::vector<Vertex> vertexes;

  void gen_vertexes(std::vector<float>& positions, std::vector<uint32_t>& colors); // Adds vertexes, based on provided coordinate and color tables
};

struct Scene {
public:
  GLuint shader_program;

};

// Stores glyph data
struct Character {
public:
  uint32_t texture_id, advance;
  int size_x, size_y, bearing_x, bearing_y;
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
  GLFWwindow* window;
  GLuint shader_program, pm_location;
  std::string vertex_shader_path;
  std::string fragment_shader_path;
  std::vector<std::unique_ptr<VObject>> vertex_objects;
  TMatrix pm;
  FontHandler font_handler;

  void set_orthographic_projection(float width, float height);

  VObject* add_vobject(std::vector<float>& positions, std::vector<uint32_t>& colors);

  void remove_vobject(VObject* vo);

  void clear_vobject(VObject* vo);

  int init(); // Creates window and defines stuff

  void window_proc(void (*additional_func)(GWindow*)); // Main loop of the program

  void clear(); // Free memory

  friend struct VObject;
};

#endif