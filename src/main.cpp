#include "glm/trigonometric.hpp"
#include <SDL2/SDL.h>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <string>
#include <vector>
// Globals
int gScreenHeight = 480;
int gScreenWidth = 640;
SDL_Window *gGraphicsAppWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

bool gQuit = false;

// VAO
GLuint gVertexArrayObj = 0;
// VBO
GLuint gVertexBufferObj = 0;
GLuint gVertexBufferObj2 = 0;

GLuint gIndexBufferObj = 0;
GLuint gIndexBufferObj2 = 0;

// Program Object (for our shaders)
GLuint gGraphicsPipelineShaderProgram = 0;

float g_uOffset = -2.0f;
float g_uRotate = 0.0f;
float g_uScale = 0.5f;

static void GLClearAllErrors() {
  while (glGetError() != GL_NO_ERROR) {
  }
}

static bool GLCheckErrorStatus(const char *function, int line) {
  while (GLenum error = glGetError()) {
    std::cout << "OpenGL Error: " << error << "\tLine: " << line
              << "\tfunction: " << function << std::endl;
    return true;
  }

  return false;
}

#define GLCheck(x)                                                             \
  GLClearAllErrors();                                                          \
  x;                                                                           \
  GLCheckErrorStatus(#x, __LINE__);

std::string LoadShaderAsString(const std::string &filename) {
  std::string result = "";

  std::string line = "";
  std::ifstream myFile(filename.c_str());

  if (myFile.is_open()) {
    while (std::getline(myFile, line)) {
      result += line + '\n';
    }
  }

  return result;
}

GLuint CompileShader(GLuint type, const std::string &source) {
  GLuint shaderObject;

  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);

  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }

  const char *src = source.c_str();
  glShaderSource(shaderObject, 1, &src, nullptr);
  glCompileShader(shaderObject);

  return shaderObject;
}

GLuint CreateShaderProgram(const std::string &vertexShaderSrc,
                           const std::string &fragmentShaderSrc) {
  GLuint programObj = glCreateProgram();

  GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc);
  GLuint myFragmentShader =
      CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);

  glAttachShader(programObj, myVertexShader);
  glAttachShader(programObj, myFragmentShader);
  glLinkProgram(programObj);

  glValidateProgram(programObj);

  return programObj;
}

void CreateGraphicsPipeline() {

  std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
  std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");
  gGraphicsPipelineShaderProgram =
      CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GetOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
}

void VertexSpecification() {

  const std::vector<GLfloat> vertexPosition{
      // x    y    z
      -0.5f, -0.5f, 0.0f, // Left vertex
      0.5f, -0.5f, 0.0f,  // Right vertex
      -0.5f, 0.5f, 0.0f,  // Top vertex
      // Second Triangle

      0.5f, 0.5f, 0.0f, // Top-right vertex

  };

  const std::vector<GLfloat> vertexColors{
      // r   g     b
      1.0f, 0.0f, 0.0f, // Left vertex pos
      0.0f, 1.0f, 0.0f, // Right vertex pos
      0.0f, 0.0f, 1.0f, // Top vertex pos
                        // second triangle
      0.0f, 0.0f, 1.0f, // Top vertex pos
  };

  glGenVertexArrays(1, &gVertexArrayObj);
  glBindVertexArray(gVertexArrayObj);

  // Setting up positions
  glGenBuffers(1, &gVertexBufferObj);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObj);
  glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat),
               vertexPosition.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

  // Setting up colors
  glGenBuffers(1, &gVertexBufferObj2);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObj2);
  glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(GLfloat),
               vertexColors.data(), GL_STATIC_DRAW);

  // Setup index buffer object
  const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1};

  glGenBuffers(1, &gIndexBufferObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint),
               indexBufferData.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, // rgb
                        GL_FLOAT, GL_FALSE, 0, (void *)0);

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
}

void InitializeProgram() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to initialize the SDL2 library\n";
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gGraphicsAppWindow = SDL_CreateWindow(
      "OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);

  if (gGraphicsAppWindow == nullptr) {
    std::cout << "SDL_Window was not able to be created" << std::endl;
    exit(1);
  }

  gOpenGLContext = SDL_GL_CreateContext(gGraphicsAppWindow);

  if (gOpenGLContext == nullptr) {
    std::cout << "OpenGL context not available\n";
    exit(1);
  }

  // Initialize the Glad Library
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    std::cout << "glad was not initialized" << std::endl;
    exit(1);
  }

  GetOpenGLVersionInfo();
}

void Input() {
  SDL_Event e;

  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      std::cout << "Goodbye!" << std::endl;
      gQuit = true;
    }
  }

  // Retrieve keyboard state
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_UP]) {
    g_uOffset += 0.01f;
    std::cout << "g_uOffset: " << g_uOffset << std::endl;
  }
  if (state[SDL_SCANCODE_DOWN]) {
    g_uOffset -= 0.01f;
    std::cout << "g_uOffset: " << g_uOffset << std::endl;
  }
  if (state[SDL_SCANCODE_LEFT]) {
    g_uRotate -= 1.0f;
    std::cout << "g_uRotate: " << g_uRotate << std::endl;
  }
  if (state[SDL_SCANCODE_RIGHT]) {
    g_uRotate += 1.0f;
    std::cout << "g_uRotate: " << g_uRotate << std::endl;
  }
}

void PreDraw() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glViewport(0, 0, gScreenWidth, gScreenHeight);
  glClearColor(1.f, 1.f, 0.1f, 1.f);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glUseProgram(gGraphicsPipelineShaderProgram);

  // Model tansformation by translating object into world space
  glm::mat4 model =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, g_uOffset));

  model =
      glm::rotate(model, glm::radians(g_uRotate), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(g_uScale, g_uScale, g_uScale));

  GLint u_ModelMatrixLocation =
      glGetUniformLocation(gGraphicsPipelineShaderProgram, "uModelMatrix");

  if (u_ModelMatrixLocation >= 0) {
    glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
    std::cout << "location of uModelMatrix: " << u_ModelMatrixLocation
              << std::endl;
  } else {
    std::cout << "Could not find uModelMatrix, maybe a mispelling?\n";
    exit(EXIT_FAILURE);
  }

  glm::mat4 perspective =
      glm::perspective(glm::radians(45.0f),
                       (float)gScreenWidth / (float)gScreenHeight, 0.1f, 10.0f);

  GLint u_ProjectionLocation =
      glGetUniformLocation(gGraphicsPipelineShaderProgram, "uProjection");
  if (u_ProjectionLocation >= 0) {
    glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &perspective[0][0]);
    std::cout << "location of uProjection: " << u_ProjectionLocation
              << std::endl;
  } else {
    std::cout << "Could not find uProjection, maybe a mispelling?\n";
    exit(EXIT_FAILURE);
  }
}

void Draw() {

  glBindVertexArray(gVertexArrayObj);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObj);

  // Render Data
  // glDrawArrays(GL_TRIANGLES, 0, 6);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void MainLoop() {
  while (!gQuit) {
    Input();

    PreDraw();
    Draw();

    // Update the screen
    SDL_GL_SwapWindow(gGraphicsAppWindow);
  }
}

void CleanUp() {
  SDL_DestroyWindow(gGraphicsAppWindow);
  SDL_Quit();
}

int main(int argc, char *argv[]) {

  InitializeProgram();

  VertexSpecification();

  CreateGraphicsPipeline();

  MainLoop();

  CleanUp();

  return 0;
}
