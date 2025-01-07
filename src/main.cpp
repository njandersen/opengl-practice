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

// Our Libraries
#include "Camera.hpp"
struct App {
  int mScreenHeight = 480;
  int mScreenWidth = 640;
  SDL_Window *mGraphicsAppWindow = nullptr;
  SDL_GLContext mOpenGLContext = nullptr;
  bool mQuit = false;
  // Program Object (for our shaders)
  GLuint mGraphicsPipelineShaderProgram = 0;
  Camera mCamera;
};

struct Transform {
  glm::vec3 translation;
};

struct Mesh3D {
  // VAO
  GLuint mVertexArrayObj = 0;
  // VBO
  GLuint mVertexBufferObj = 0;
  GLuint mVertexBufferObj2 = 0;

  GLuint mIndexBufferObj = 0;
  GLuint mIndexBufferObj2 = 0;

  GLuint mPipeline = 0;

  Transform mTransform;
  float m_uRotate = 0.0f;
  float m_uScale = 0.5f;
};

// Globals
App gApp;
Mesh3D gMesh1;
Mesh3D gMesh2;

void MeshDelete(Mesh3D *mesh) {
  glDeleteBuffers(1, &mesh->mVertexBufferObj);
  glDeleteBuffers(1, &mesh->mVertexBufferObj2);
}

void MeshSetPipeline(Mesh3D *mesh, GLuint pipeline) {

  mesh->mPipeline = pipeline;
}

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
  gApp.mGraphicsPipelineShaderProgram =
      CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GetOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << std::endl;
}

void MeshCreate(Mesh3D *mesh) {

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

  glGenVertexArrays(1, &mesh->mVertexArrayObj);
  glBindVertexArray(mesh->mVertexArrayObj);

  // Setting up positions
  glGenBuffers(1, &mesh->mVertexBufferObj);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObj);
  glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat),
               vertexPosition.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void *)0);

  // Setting up colors
  glGenBuffers(1, &mesh->mVertexBufferObj2);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->mVertexBufferObj2);
  glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(GLfloat),
               vertexColors.data(), GL_STATIC_DRAW);

  // Setup index buffer object
  const std::vector<GLuint> indexBufferData{2, 0, 1, 3, 2, 1};

  glGenBuffers(1, &mesh->mIndexBufferObj);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->mIndexBufferObj);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint),
               indexBufferData.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, // rgb
                        GL_FLOAT, false, 0, (void *)0);

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
}

void InitializeProgram(App *app) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Failed to initialize the SDL2 library\n";
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  app->mGraphicsAppWindow = SDL_CreateWindow(
      "OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      app->mScreenWidth, app->mScreenHeight, SDL_WINDOW_OPENGL);

  if (app->mGraphicsAppWindow == nullptr) {
    std::cout << "SDL_Window was not able to be created" << std::endl;
    exit(1);
  }

  app->mOpenGLContext = SDL_GL_CreateContext(app->mGraphicsAppWindow);

  if (app->mOpenGLContext == nullptr) {
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
  static int mouseX = gApp.mScreenWidth / 2;
  static int mouseY = gApp.mScreenHeight / 2;

  SDL_Event e;

  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      std::cout << "Goodbye!" << std::endl;
      gApp.mQuit = true;
    } else if (e.type == SDL_MOUSEMOTION) {
      mouseX += e.motion.xrel;
      mouseY += e.motion.yrel;
      gApp.mCamera.MouseLook(mouseX, mouseY);
    }
  }

  // Retrieve keyboard state
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  float speed = 0.1f;
  if (state[SDL_SCANCODE_E]) {
    gApp.mCamera.MoveForward(speed);
    // g_uOffset += 0.01f;
    // std::cout << "g_uOffset: " << g_uOffset << std::endl;
  }
  if (state[SDL_SCANCODE_D]) {
    gApp.mCamera.MoveBackward(speed);
  }
  if (state[SDL_SCANCODE_S]) {
    gApp.mCamera.MoveLeft(speed);
  }
  if (state[SDL_SCANCODE_F]) {
    gApp.mCamera.MoveRight(speed);
  }
  if (state[SDL_SCANCODE_ESCAPE]) {
    gApp.mQuit = true;
  }
}

// Returns location of uniform var based on its name
int FindUniformLocation(GLuint pipeline, const GLchar *name) {
  GLint location = glGetUniformLocation(pipeline, name);
  if (location < 0) {
    std::cerr << "Could not find " << name << ", maybe a mispelling?\n";
    exit(EXIT_FAILURE);
  }
  return location;
}

void MeshDraw(Mesh3D *mesh) {
  if (mesh == nullptr) {
    return;
  }

  // Setup which graphics pipeline we are going to use
  glUseProgram(mesh->mPipeline);

  mesh->m_uRotate -= 0.1f;

  // Model tansformation by translating object into world space
  glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                   glm::vec3(mesh->mTransform.translation.x,
                                             mesh->mTransform.translation.y,
                                             mesh->mTransform.translation.z));

  model = glm::rotate(model, glm::radians(mesh->m_uRotate),
                      glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model,
                     glm::vec3(mesh->m_uScale, mesh->m_uScale, mesh->m_uScale));

  GLint u_ModelMatrixLocation =
      FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "uModelMatrix");
  glUniformMatrix4fv(u_ModelMatrixLocation, 1, false, &model[0][0]);

  glm::mat4 view = gApp.mCamera.GetViewMatrix();
  GLint u_ViewLocation =
      FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "uViewMatrix");
  glUniformMatrix4fv(u_ViewLocation, 1, false, &view[0][0]);

  glm::mat4 perspective = gApp.mCamera.GetProjectionMatrix();

  GLint u_ProjectionLocation =
      FindUniformLocation(gApp.mGraphicsPipelineShaderProgram, "uProjection");
  glUniformMatrix4fv(u_ProjectionLocation, 1, false, &perspective[0][0]);

  // Enable our attributes
  glBindVertexArray(mesh->mVertexArrayObj);

  // Render Data
  // glDrawArrays(GL_TRIANGLES, 0, 6);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // Stop using our current graphics pipeline
  glUseProgram(0);
}

void MainLoop() {
  SDL_WarpMouseInWindow(gApp.mGraphicsAppWindow, gApp.mScreenWidth / 2,
                        gApp.mScreenHeight / 2);
  SDL_SetRelativeMouseMode(SDL_TRUE);
  while (!gApp.mQuit) {
    Input();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
    glClearColor(1.f, 1.f, 0.1f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    MeshDraw(&gMesh1);

    MeshDraw(&gMesh2);

    // Update the screen
    SDL_GL_SwapWindow(gApp.mGraphicsAppWindow);
  }
}

void CleanUp() {
  SDL_DestroyWindow(gApp.mGraphicsAppWindow);
  gApp.mGraphicsAppWindow = nullptr;

  // Delete opengl objects
  MeshDelete(&gMesh1);

  // Delete graphics pipeline
  glDeleteProgram(gApp.mGraphicsPipelineShaderProgram);
  SDL_Quit();
}

int main(int argc, char *argv[]) {

  InitializeProgram(&gApp);

  // Setup camera
  gApp.mCamera.SetProjectionMatrix(
      glm::radians(45.0f), (float)gApp.mScreenWidth / (float)gApp.mScreenHeight,
      0.1f, 10.0f);

  MeshCreate(&gMesh1);
  gMesh1.mTransform.translation.x = 0.0f;
  gMesh1.mTransform.translation.y = 0.0f;
  gMesh1.mTransform.translation.z = -2.0f;

  MeshCreate(&gMesh2);
  gMesh2.mTransform.translation.x = 2.0f;
  gMesh2.mTransform.translation.y = 0.0f;
  gMesh2.mTransform.translation.z = -2.0f;

  CreateGraphicsPipeline();

  MeshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
  MeshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);

  MainLoop();

  CleanUp();

  return 0;
}
