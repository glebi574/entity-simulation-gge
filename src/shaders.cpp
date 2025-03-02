#include "g-utils.h"

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
  const char* src = shader_code.c_str();
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

void GWindow::new_shader_program(GLuint* sp, std::string path_vs, std::string path_fs) {
  *sp = create_shader_program(path_vs, path_fs);
}