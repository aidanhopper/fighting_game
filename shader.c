#include "shader.h"

char *readfile(const char *path) {
  FILE *f = fopen(path, "rt");
  assert(f);
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = (char *)malloc(length + 1);
  fread(buffer, 1, length, f);
  buffer[length - 1] = '\0';
  fclose(f);
  return buffer;
}

u32 createshader(const char *vertexpath, const char *fragmentpath) {
  char *vertexcode = readfile(vertexpath);
  const char *vcode = vertexcode;
  u32 vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vcode, NULL);
  glCompileShader(vertex);
  free(vertexcode);
  vertexcode = NULL;

  char *fragmentcode = readfile(fragmentpath);
  const char *fcode = fragmentcode;
  u32 fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fcode, NULL);
  glCompileShader(fragment);
  free(fragmentcode);
  fragmentcode = NULL;

  u32 id = glCreateProgram();
  glAttachShader(id, vertex);
  glAttachShader(id, fragment);
  glLinkProgram(id);

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return id;
}
