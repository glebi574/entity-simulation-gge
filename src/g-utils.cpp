#include "g-utils.h"

uint32_t rgba_to_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return ((r * 256 + g) * 256 + b) * 256 + a;
}

void VObject::gen_vertexes(std::vector<float>& positions, std::vector<uint32_t>& colors) {
  for (int i = 0; i < colors.size(); ++i) {
    float pos[3] = { positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2] };
    vertexes.emplace_back(Vertex(pos, colors[i]));
  }
}

std::string load_shader(std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Failed to open shader file: " << path << std::endl;
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  return buffer.str();
}

GLuint compile_shader(GLenum shader_type, std::string& shader_code) {
  GLuint shader = glCreateShader(shader_type);
  const char *src = shader_code.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  int success;
  char log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << log << std::endl;
  }

  return shader;
}

GLuint create_shader_program(std::string& vertex_shader_path, std::string& fragment_shader_path) {
  std::string vertex_source = load_shader(vertex_shader_path);
  std::string fragment_source = load_shader(fragment_shader_path);
  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
  GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  int success;
  char log[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, nullptr, log);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << log << std::endl;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  GWindow* gw = static_cast<GWindow*>(glfwGetWindowUserPointer(window));
  gw->set_orthographic_projection(width, height);

  glViewport(0, 0, width, height);
}

VObject* Scene::add_vobject(std::vector<float>& positions, std::vector<uint32_t>& colors) {
  vertex_objects.emplace_back(std::make_unique<VObject>());
  VObject* vo = vertex_objects.back().get();
  vo->gen_vertexes(positions, colors);

  glGenBuffers(1, &vo->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vo->VBO);
  glBufferData(GL_ARRAY_BUFFER, vo->vertexes.size() * sizeof(Vertex), vo->vertexes.data(), GL_STATIC_DRAW);

  vo->vpos_location = glGetAttribLocation(shader_program, "vPos");
  vo->vcol_location = glGetAttribLocation(shader_program, "vCol");
  vo->tm_location   = glGetUniformLocation(shader_program, "tm");

  glGenVertexArrays(1, &vo->VAO);
  glBindVertexArray(vo->VAO);

  glEnableVertexAttribArray(vo->vpos_location);
  glVertexAttribPointer(vo->vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

  glEnableVertexAttribArray(vo->vcol_location);
  glVertexAttribPointer(vo->vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

  return vo;
}

Scene::Scene(std::string path_vs, std::string path_fs) {
  shader_program = create_shader_program(path_vs, path_fs);
}

Scene* GWindow::add_scene(std::string path_vs, std::string path_fs) {
  scenes.emplace_back(std::make_unique<Scene>(path_vs, path_fs));
  Scene* sp = scenes.back().get();

  pm_location = glGetUniformLocation(sp->shader_program, "pm"); // Keep pm location constant ig

  return sp;
}

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

void Scene::clear_vobject(VObject* vo) {
  glDeleteVertexArrays(1, &vo->VAO);
  glDeleteBuffers(1, &vo->VBO);
}

void Scene::remove_vobject(VObject* vo) {
  clear_vobject(vo);
  for (int i = 0; i < vertex_objects.size(); ++i)
    if (vertex_objects[i].get() == vo) {
      vertex_objects.erase(vertex_objects.begin() + i);
      return;
    }
}

int GWindow::init() {
  if (!glfwInit())
    return 1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  bool fullscreen = false;

  if (fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    window = glfwCreateWindow(mode->width, mode->height, "Window", monitor, NULL);
  }
  else
    window = glfwCreateWindow(800, 600, "Window", NULL, NULL);

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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //if (font_handler.init())
  //  return 1;

  set_orthographic_projection(800, 600);

  return 0;
}

void GWindow::window_proc(void (*additional_func)(GWindow*)) {
  while (!glfwWindowShouldClose(window)) {
    additional_func(this);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniformMatrix4fv(pm_location, 1, GL_FALSE, &pm.m[0][0]);

    for (std::unique_ptr<Scene>& scene : scenes) {
      Scene* sp = scene.get();
      glUseProgram(sp->shader_program);

      for (std::unique_ptr<VObject>& uvo : sp->vertex_objects) {
        VObject* vo = uvo.get();
        glBindVertexArray(vo->VAO);
        glUniformMatrix4fv(vo->tm_location, 1, GL_FALSE, &vo->tm.m[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, vo->vertexes.size());
      }
    }

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
}

void GWindow::clear() {
  for (std::unique_ptr<Scene>& scene : scenes) {
    Scene* sp = scene.get();
    for (std::unique_ptr<VObject>& uvo : sp->vertex_objects)
      sp->clear_vobject(uvo.get());
    glDeleteProgram(sp->shader_program);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}

