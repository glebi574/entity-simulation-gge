#include "g-utils.h"

// Updates scales on resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  GWindow* gw = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
  gw->set_orthographic_projection(width, height);

  glViewport(0, 0, width, height);
  gw->width = width;
  gw->height = height;
  gw->size_ratio = static_cast<double>(width) / static_cast<double>(height);
}

void GWindow::set_orthographic_projection(float width, float height) {
  float cleft = -1000,
    cright = 1000,
    cbottom = -1000,
    ctop = 1000,
    cnear = -1000,
    cfar = 1000;
  float scale = width / height;

  if (width < height) {
    cleft *= scale;
    cright *= scale;
  }
  else if (width > height) {
    cbottom /= scale;
    ctop /= scale;
  }

  pm.m[0][0] = 2 / (cright - cleft);
  pm.m[1][1] = 2 / (ctop - cbottom);
  pm.m[2][2] = 2 / (cfar - cnear);

  pm.m[0][3] = (cleft + cright) / (cleft - cright);
  pm.m[1][3] = (cbottom + ctop) / (cbottom - ctop);
  pm.m[2][3] = (cnear + cfar) / (cnear - cfar);
}

// Creates window and defines some stuff
int GWindow::init() {
  if (!glfwInit())
    return 1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  bool fullscreen = false;

  int x = 600, y = 600;
  width = x;
  height = y;
  if (fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    window = glfwCreateWindow(mode->width, mode->height, "Window", monitor, NULL);
  }
  else
    window = glfwCreateWindow(x, y, "Window", NULL, NULL);

  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  GLenum state = glewInit();

  if (state != GLEW_OK) {
    std::cerr << "Failed to initialize GLEW; " << "error code: " << state << "; " << glewGetErrorString(state) << std::endl;
    return 1;
  }

  new_shader_program(&triangle_sp, "triangle_vs.glsl", "triangle_fs.glsl");
  new_shader_program(&glyph_sp, "glyph_vs.glsl", "glyph_fs.glsl");
  pm_location = glGetUniformLocation(triangle_sp, "pm");

  tvo_data.tm_location = glGetUniformLocation(triangle_sp, "tm");
  tvo_data.vpos_location = glGetAttribLocation(triangle_sp, "vPos");
  tvo_data.vcol_location = glGetAttribLocation(triangle_sp, "vCol");

  gvo_data.tcol_location = glGetUniformLocation(glyph_sp, "tCol");

  gvo_data.tm_location = glGetUniformLocation(glyph_sp, "tm");
  gvo_data.vpos_location = glGetAttribLocation(glyph_sp, "vPos");
  gvo_data.vtex_location = glGetAttribLocation(glyph_sp, "vTex");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (font_handler.init())
    return 1;

  set_orthographic_projection(x, y);

  return 0;
}

void GWindow::get_cursor_pos(double& x, double& y) {
  glfwGetCursorPos(window, &x, &y);
  x =  x / width  * 2000 - 1000;
  y = -y / height * 2000 + 1000;
  if (size_ratio < 1.0)
    x *= size_ratio;
  else
    y /= size_ratio;
}

void GWindow::draw_tvo() {
  glUseProgram(triangle_sp);
  glUniformMatrix4fv(pm_location, 1, GL_FALSE, &pm.m[0][0]);
  for (std::unique_ptr<TriangleVO>& uvo : tvo) {
    TriangleVO* vo = uvo.get();
    glBindVertexArray(vo->VAO);
    glUniformMatrix4fv(tvo_data.tm_location, 1, GL_FALSE, &vo->tm.m[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, vo->vertexes.size());
  }
}

void GWindow::draw_gvo() {
  glUseProgram(glyph_sp);
  glUniformMatrix4fv(pm_location, 1, GL_FALSE, &pm.m[0][0]);
  for (std::unique_ptr<TextVO>& uvo : gvo) {
    TextVO* vo = uvo.get();
    glUniform4f(gvo_data.tcol_location, vo->color[0], vo->color[1], vo->color[2], vo->color[3]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vo->VAO);
    glUniformMatrix4fv(gvo_data.tm_location, 1, GL_FALSE, &vo->tm.m[0][0]);
    int offset = 0;
    for (std::string::const_iterator c = vo->str.begin(); c != vo->str.end(); ++c) {
      Character ch = font_handler.characters[*c];
      glBindTexture(GL_TEXTURE_2D, ch.texture_id);
      glDrawArrays(GL_TRIANGLES, offset, 6);
      offset += 6;
    }
  }
}

void GWindow::window_proc() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  draw_tvo();

  draw_gvo();

  glfwSwapBuffers(window);

  glfwPollEvents();
}

void GWindow::clear() {
  for (std::unique_ptr<TriangleVO>& uvo : tvo)
    uvo->clear();
  for (std::unique_ptr<TextVO>& uvo : gvo)
    uvo->clear();
  glDeleteProgram(triangle_sp);
  glDeleteProgram(glyph_sp);

  glfwDestroyWindow(window);
  glfwTerminate();
}

TriangleVO* GWindow::add_tvo(std::vector<float>& positions, std::vector<uint32_t>& colors) {
  tvo.emplace_back(std::make_unique<TriangleVO>());
  TriangleVO* vo = tvo.back().get();
  vo->gen_vertexes(positions, colors);

  glGenBuffers(1, &vo->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vo->VBO);
  glBufferData(GL_ARRAY_BUFFER, vo->vertexes.size() * sizeof(Vertex), vo->vertexes.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vo->VAO);
  glBindVertexArray(vo->VAO);

  glEnableVertexAttribArray(tvo_data.vpos_location);
  glVertexAttribPointer(tvo_data.vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

  glEnableVertexAttribArray(tvo_data.vcol_location);
  glVertexAttribPointer(tvo_data.vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  return vo;
}

TextVO* GWindow::add_text(std::string str, float x, float y, float scale, uint32_t color) {
  gvo.emplace_back(std::make_unique<TextVO>());
  TextVO* vo = gvo.back().get();
  uint32_t offset = 0;
  for (std::string::const_iterator c = str.begin(); c != str.end(); ++c) {
    Character ch = font_handler.characters[*c];
    vo->gen_vertexes(ch, offset);
    offset += ch.advance >> 6;
  }
  vo->str = str;
  vo->set_hex_color(color);
  vo->tm.set_scale(scale);
  vo->tm.set_offset(x, y, 0);

  glGenBuffers(1, &vo->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vo->VBO);
  glBufferData(GL_ARRAY_BUFFER, vo->vertexes.size() * sizeof(Glyph), vo->vertexes.data(), GL_STATIC_DRAW);

  glGenVertexArrays(1, &vo->VAO);
  glBindVertexArray(vo->VAO);

  glEnableVertexAttribArray(gvo_data.vpos_location);
  glVertexAttribPointer(gvo_data.vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Glyph), (void*)offsetof(Glyph, pos));

  glEnableVertexAttribArray(gvo_data.vtex_location);
  glVertexAttribPointer(gvo_data.vtex_location, 2, GL_FLOAT, GL_FALSE, sizeof(Glyph), (void*)offsetof(Glyph, quad));

  return vo;
}

void GWindow::remove_vo(VObject* vo) {
  vo->clear();
  for (int i = 0; i < tvo.size(); ++i)
    if (tvo[i].get() == vo) {
      tvo.erase(tvo.begin() + i);
      return;
    }
  for (int i = 0; i < gvo.size(); ++i)
    if (gvo[i].get() == vo) {
      gvo.erase(gvo.begin() + i);
      return;
    }
}