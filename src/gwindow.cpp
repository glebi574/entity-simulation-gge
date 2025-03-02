#include "g-utils.h"

// Updates scales on resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  GWindow* gw = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
  gw->set_orthographic_projection(width, height);

  glViewport(0, 0, width, height);
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

  int x = 800, y = 600;
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
  pm_location = glGetUniformLocation(triangle_sp, "pm");

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (font_handler.init())
    return 1;

  set_orthographic_projection(x, y);

  return 0;
}

void GWindow::window_proc(void (*additional_func)(GWindow*)) {
  while (!glfwWindowShouldClose(window)) {
    additional_func(this);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformMatrix4fv(pm_location, 1, GL_FALSE, &pm.m[0][0]);

    glUseProgram(triangle_sp);
    for (std::unique_ptr<TriangleVO>& uvo : tvo) {
      TriangleVO* vo = uvo.get();
      glBindVertexArray(vo->VAO);
      glUniformMatrix4fv(vo->tm_location, 1, GL_FALSE, &vo->tm.m[0][0]);
      glDrawArrays(GL_TRIANGLES, 0, vo->vertexes.size());
    }
    /*
    glUseProgram(glyph_sp);
    for (std::unique_ptr<GlyphVO>& uvo : gvo) {
      GlyphVO* vo = uvo.get();
      glBindVertexArray(vo->VAO);
      glUniformMatrix4fv(vo->tm_location, 1, GL_FALSE, &vo->tm.m[0][0]);
      // glDrawArrays(GL_TRIANGLES, 0, vo->vertexes.size());
    }*/

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
}

void GWindow::clear() {
  for (std::unique_ptr<TriangleVO>& uvo : tvo)
    clear_vo(uvo.get());
  for (std::unique_ptr<GlyphVO>& uvo : gvo)
    clear_vo(uvo.get());
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

  vo->tm_location = glGetUniformLocation(triangle_sp, "tm");
  vo->vpos_location = glGetAttribLocation(triangle_sp, "vPos");
  vo->vcol_location = glGetAttribLocation(triangle_sp, "vCol");

  glGenVertexArrays(1, &vo->VAO);
  glBindVertexArray(vo->VAO);

  glEnableVertexAttribArray(vo->vpos_location);
  glVertexAttribPointer(vo->vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

  glEnableVertexAttribArray(vo->vcol_location);
  glVertexAttribPointer(vo->vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  return vo;
}

GlyphVO* GWindow::add_gvo() {

}

void GWindow::clear_vo(VObject* vo) {
  glDeleteVertexArrays(1, &vo->VAO);
  glDeleteBuffers(1, &vo->VBO);
}

void GWindow::remove_vo(VObject* vo) {
  clear_vo(vo);
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