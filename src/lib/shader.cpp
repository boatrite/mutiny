#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
  GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
  GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
  compileAndLinkProgram(vertex, fragment);
}

void Shader::use() {
  glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

GLuint Shader::compileShader(const char* path, GLenum type) {
  // 1. Read shader file
  std::string shaderCode;
  std::ifstream shaderFile;

  // Ensure ifstream objects can throw exceptions
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    shaderFile.open(path);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderStream.str();
  } catch (std::ifstream::failure e) {
    std::cerr << "ERROR: Shader file not successfully read. Path: " << path << std::endl;
  }

  const char* code = shaderCode.c_str();

  // 2. Compile shader code
  GLuint shader;

  int success;
  char infoLog[512];

  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR: Shader compilation failed. Path: " << path << std::endl;
  }

  return shader;
}

void Shader::compileAndLinkProgram(GLuint vertex, GLuint fragment) {
  // Create Shader Program
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);

  int success;
  char infoLog[512];

  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cerr << "ERROR: Program linking failed" << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}