#include "doomdef.h"
#include <GL/gl.h>
#include <GL/glext.h>

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

struct {
  SDL_Window *window;
  SDL_GLContext context;
  bool quit;
} state;

void render() {}

void setup() {
  // initialize SDL
  ASSERT(!SDL_Init(SDL_INIT_VIDEO), "SDL failed to intialize: %s\n",
         SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // intializse window
  state.window =
      SDL_CreateWindow("TEST", SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                       SDL_WINDOWPOS_CENTERED_DISPLAY(0), SCREEN_WIDTH,
                       SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
  // initialize opengl context
  state.context = SDL_GL_CreateContext(state.window);
}

int main() {
  // do setup

  setup();

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left
      0.5f,  -0.3f, 0.0f, // right
      0.0f,  0.5f,  0.0f  // top
  };

  // Shader stuff
  u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  u32 shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // idk
  u32 VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // loop
  state.quit = false;
  while (!state.quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        state.quit = true;
        break;
      }
    }

    // set color to black
    glClearColor(1, 0, 1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // draw
    glColor3f(0.0, 0.0, 0.0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(state.window);
  }

  // clean up
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  SDL_GL_DeleteContext(state.context);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
  return 0;
}
