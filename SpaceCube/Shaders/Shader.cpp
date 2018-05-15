#include "Shader.h"

Shader::Shader() {

}

void Shader::create(string files) {
  _v_shaderID = compileShader(files + ".vert", GL_VERTEX_SHADER);
  _f_shaderID = compileShader(files + ".frag", GL_FRAGMENT_SHADER);

  _pID = glCreateProgram();
  glAttachShader(_pID, _f_shaderID);
  glAttachShader(_pID, _v_shaderID);
  glLinkProgram(_pID);
  glValidateProgram(_pID);

  checkError(_pID, GL_LINK_STATUS, true, "Shader link failed");

  cout << "Created shader program " << _pID << endl;
}

void Shader::bind() {
  glUseProgram(_pID);
}

Shader::~Shader() {
  cout << "Delete shader program " << _pID << endl;
  glDetachShader(_pID, _v_shaderID);
  glDetachShader(_pID, _f_shaderID);

  glDeleteProgram(_pID);
}
