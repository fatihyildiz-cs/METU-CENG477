#include "helper.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"

static GLFWwindow* win = NULL;
int widthWindow = 1000, heightWindow = 1000;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightTexture;
GLuint idMVPMatrix;

// Buffers
GLuint idVertexBuffer;
GLuint idIndexBuffer;

int textureWidth, textureHeight;
float heightFactor = 10.0f;

// I ADDED THE FOLLOWING VARIABLES, AND ALSO THE CAMERA STRUCT. I TOOK THE VALUES FROM THE PDF.

// I THINK WE WILL ALLOCATE MEMORY OF SIZE 2*w*d triangles = 3*2*w*d vertices  TO THE vertexData BELOW
glm::vec3 *vertexData;

GLfloat fieldOfView = 45;
GLfloat aspectRatio = 1;
GLfloat nearPlaneDistance = 0.1;
GLfloat farPlaneDistance = 1000;

struct Camera{
  glm::vec3 position= glm::vec3(textureWidth / 2, textureWidth / 10, (-1) * (textureWidth / 4));
  glm::vec3 up= glm::vec3(0, 1, 0);
  glm::vec3 gaze= glm::vec3(0, 0, 1);
  glm::vec3 left= glm::cross(up, gaze);
  GLfloat speed = 0.0f;
};

// I DON'T KNOW WHAT THE INITIALCAMERA IS. I JUST IGNORED IT FOR NOW.
Camera camera;
Camera initialCamera;

glm::vec3 lightPos;
GLuint depthMapFBO;
GLuint depthCubemap;
bool lightPosFlag = false;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void cameraSpecifications() {


    glm::mat4 projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);

    glm::mat4 viewMatrix = glm::lookAt(camera.position, glm::vec3(camera.position + camera.gaze * nearPlaneDistance), camera.up);

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    glm::mat4 normal_matrix = glm::transpose(glm::inverse(viewMatrix));

    GLint mvpLocation = glGetUniformLocation(idProgramShader, "MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

    GLint mvLocation = glGetUniformLocation(idProgramShader, "MV");
    glUniformMatrix4fv(mvLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    GLint normalMatrixLocation = glGetUniformLocation(idProgramShader, "normalMatrix");
    glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, &normal_matrix[0][0]);

    GLint cameraPositionLocation = glGetUniformLocation(idProgramShader, "cameraPosition");
    glUniform3fv(cameraPositionLocation, 1, &camera.position[0]);

}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Please provide height and texture image files!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This might be used for local

  win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);

  if (!win) {
      glfwTerminate();
      exit(-1);
  }
  glfwMakeContextCurrent(win);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

      glfwTerminate();
      exit(-1);
  }

  // THESE 4 LINES ATTACH THE SHADERS IN THE DIRECTORY TO THE PROGRAM
  std::string vertexShader = "shader.vert";
  std::string fragmentShader = "shader.frag";
  initShaders(idProgramShader, vertexShader, fragmentShader);
  glUseProgram(idProgramShader);

  glfwSetKeyCallback(win, keyCallback);

  initTexture(argv[1], argv[2], &textureWidth, &textureHeight);
  
  int vertexCount = 6 * textureWidth * textureHeight;

  vertexData = new glm::vec3[vertexCount]; // 2*w*d triangles and 3*2*w*d vertices



  while(!glfwWindowShouldClose(win)) {
    glfwSwapBuffers(win);
    glfwPollEvents();
  }


  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
