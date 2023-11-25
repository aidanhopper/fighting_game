#include "doomdef.h"
#include "shader.h"

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

  /*
  // positions          // colors           // texture coords
  float vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  */
  float vertices[] = {
      0.5f,  0.5f,  0.0, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // coords, color, tex
      0.5f,  -0.5f, 0.0, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // coords, color, tex
      -0.5f, -0.5f, 0.0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // coords, color, tex
      -0.5f, 0.5f,  0.0, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // coords, color, tex
  };
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3, // second triangle
  };

  u32 shaderid = createshader("./vertexshader.glsl", "./fragmentshader.glsl");

  u32 VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glUseProgram(shaderid);

  u32 texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SDL_Surface *image = IMG_Load("./pics/wood.png");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(image);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //  loop
  state.quit = false;
  while (!state.quit) {
    u32 frameStart = SDL_GetTicks();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        state.quit = true;
        break;
      }
    }

    // set color to black
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(state.window);

    u32 frameEnd = SDL_GetTicks();
    if (frameEnd - frameStart < 1000.0 / (float)FPS)
      SDL_Delay((1000.0 / (float)FPS) - (frameEnd - frameStart));
  }

  // clean up
  SDL_GL_DeleteContext(state.context);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
  return 0;
}
