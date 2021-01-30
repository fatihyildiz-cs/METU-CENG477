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

// these values will be loaded @initTexture function call.
int textureWidth, textureHeight;

float heightFactor = 10.0f;

GLFWmonitor *monitor;
const GLFWvidmode *vidmode;


// I ADDED THE FOLLOWING VARIABLES, AND ALSO THE CAMERA STRUCT. I TOOK THE VALUES FROM THE PDF.

// WE WILL ALLOCATE MEMORY OF SIZE 2*w*d triangles = 3*2*w*d vertices  TO THE vertexData BELOW at createmeshes function
glm::vec3 *vertexData;
glm::vec2 *textureCoordData;
int vertexCount;

GLfloat fieldOfView = 45;
GLfloat aspectRatio = 1;
GLfloat nearPlaneDistance = 0.1;
GLfloat farPlaneDistance = 1000;

bool viewPortChanged = false;
bool fullscreenOn = false;
int displayWidth = 0;
int displayHeight = 0;

struct Camera{
  glm::vec3 position;
  glm::vec3 up= glm::vec3(0, 1, 0);
  glm::vec3 gaze= glm::vec3(0, 0, 1);
  glm::vec3 left= glm::cross(up, gaze);
  GLfloat speed = 0.0f;
};

Camera camera;

glm::vec3 lightPos;
bool lightPosFlag = false;

static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        heightFactor += 0.5;
        GLint heightFactorLocation = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(heightFactorLocation, heightFactor);
    }
    if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        heightFactor -= 0.5;
        GLint heightFactorLocation = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(heightFactorLocation, heightFactor);
    }
    if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPos .y-= 5;
        GLint lightPosLocation = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLocation,1, &lightPos[0]);
    }
    if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPos .y+= 5;
        GLint lightPosLocation = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLocation,1, &lightPos[0]);
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPos .x-= 5;
        GLint lightPosLocation = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLocation,1, &lightPos[0]);
    }
    if (key == GLFW_KEY_LEFT&& (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPos .x+= 5;
        GLint lightPosLocation = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLocation,1, &lightPos[0]);
    }
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPos .z+= 5;
        GLint lightPosLocation = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLocation,1, &lightPos[0]);
    }
    if (key == GLFW_KEY_DOWN&& (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPos .z-= 5;
        GLint lightPosLocation = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLocation,1, &lightPos[0]);
    }

    // PUT THE CAMERA TO INITIAL POSITION AND SPEED
    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.position = glm::vec3(textureWidth / 2, textureWidth / 10, (-1) * (textureWidth / 4));
        camera.up = glm::vec3(0, 1, 0);
        camera.gaze = glm::vec3(0, 0, 1);
        camera.left = glm::cross(camera.up, camera.gaze);
        camera.speed = 0.0f;
    }

    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.position.x -= 1;
    }
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.position.x += 1;
    }

    if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.speed += 0.01f;
    }

    if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.speed -= 0.01f;
    }

    // STOP THE CAMERA
    if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.speed = 0.0f;
    }

    // PITCH ANGLE
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.gaze = glm::rotate(camera.gaze, 0.05f, camera.left);
        camera.up = glm::rotate(camera.up, 0.05f, camera.left);
    }

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.gaze = glm::rotate(camera.gaze, -0.05f, camera.left);
        camera.up = glm::rotate(camera.up, -0.05f, camera.left);
    }

    // YAW ANGLE
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.gaze = glm::rotate(camera.gaze, 0.05f, camera.up);
        camera.left = glm::rotate(camera.left, 0.05f, camera.up);
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camera.gaze = glm::rotate(camera.gaze, -0.05f, camera.up);
        camera.left = glm::rotate(camera.left, -0.05f, camera.up);
    }

    // TOGGLE FULLSCREEN MODE
    if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT)) {

        if (fullscreenOn) {
            displayWidth = 1000;
            displayHeight = 1000;
            glfwSetWindowMonitor(window, nullptr, 0, 0, 1000, 1000, 0);

            fullscreenOn = false;
        } 
        else {
            glfwSetWindowMonitor(window, monitor ,0,0, vidmode->width, vidmode->height,vidmode->refreshRate);
            displayWidth = vidmode->width;
            displayHeight = vidmode->height;

            fullscreenOn = true;
        }
        viewPortChanged = true;
    }

}

void cameraSetup() {

    glm::mat4 projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
    glm::mat4 viewMatrix = glm::lookAt(camera.position, camera.position + glm::vec3(camera.gaze.x * nearPlaneDistance, camera.gaze.y * nearPlaneDistance, camera.gaze.z * nearPlaneDistance), camera.up);
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 modelViewProjection = projectionMatrix * viewMatrix * modelMatrix;
  
    GLint modelViewProjectionMatrixLocation = glGetUniformLocation(idProgramShader, "modelViewProjection");
    glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_FALSE, &modelViewProjection[0][0]);

    GLint modelViewMatrixLocation = glGetUniformLocation(idProgramShader, "modelView");
    glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);

    GLint normalMatrixLocation = glGetUniformLocation(idProgramShader, "invtransmatrix");
    glm::mat4 invtransview = glm::transpose(glm::inverse(viewMatrix));
    glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, &invtransview[0][0]);
    
    GLint cameraPositionLocation = glGetUniformLocation(idProgramShader, "cameraPosition");
    glUniform3fv(cameraPositionLocation, 1, &camera.position[0]);

}

// THIS CREATES THE TRIANGLES

void meshTerrain() {

    vertexCount = 6 * textureWidth * textureHeight;
    int textureCoordCount=6*textureHeight*textureWidth;
    vertexData = new glm::vec3[vertexCount]; // 2*w*d triangles and 3*2*w*d vertices
    textureCoordData=new glm::vec2 [textureCoordCount];
    int i, j, index;
    index = 0;
    glm::vec3 v1, v2, v3, v4;
    for (i = 0; i < textureWidth; i++) {
        for (j = 0; j < textureHeight; j++) {

            vertexData[index] = glm::vec3(i, 0, j);
            textureCoordData[index++]=glm::vec2 (1-((float)i/(textureWidth+1)),1-((float)j/(textureHeight+1)));
            vertexData[index ] = glm::vec3(i + 1, 0, j + 1);
            textureCoordData[index++]=glm::vec2 ((1-(float)(i+1)/(textureWidth+1)),1-((float)(j+1)/(textureHeight+1)));
            vertexData[index ] = glm::vec3(i + 1, 0, j);
            textureCoordData[index++]=glm::vec2 (1-((float)(i+1)/(textureWidth+1)),1-((float)(j)/(textureHeight+1)));
            vertexData[index] = glm::vec3(i, 0, j);
            textureCoordData[index++]=glm::vec2 (1-((float)(i)/(textureWidth+1)),1-((float)(j)/(textureHeight+1)));
            vertexData[index] = glm::vec3(i, 0, j + 1);
            textureCoordData[index++]=glm::vec2 (1-((float)(i)/(textureWidth+1)),1-((float)(j+1)/(textureHeight+1)));
            vertexData[index ] = glm::vec3(i + 1, 0, j + 1);
            textureCoordData[index++]=glm::vec2 (1-((float)(i+1)/(textureWidth+1)),1-((float)(j+1)/(textureHeight+1)));

        }
    }
}

void clearColorDepthStencilBuffers(){

    glClearStencil(0);
    glClearDepth(1.0f);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

}
void reshape(GLFWwindow* window, int width, int height)
{
    width=(width<=0)?1:width;
    height=(height<=0)?1:height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void sendVertexData(){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, vertexData);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, textureCoordData);
    glTexCoordPointer(2,GL_FLOAT,0,textureCoordData);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void sendUniformValuesToShaders(){

    // bind the 0'th texture unit to the color sampler
    GLint colorSamplerLocation = glGetUniformLocation(idProgramShader, "textureSamplerForColor");
    glUniform1i(colorSamplerLocation, 0);

    // bind the 1'st texture unit to the height sampler
    GLint heightSamplerLocation = glGetUniformLocation(idProgramShader, "textureSamplerForHeight");
    glUniform1i(heightSamplerLocation, 1);

    GLint textureWidthLocation = glGetUniformLocation(idProgramShader, "textureWidth");
    glUniform1i(textureWidthLocation, textureWidth);

    GLint textureHeightLocation = glGetUniformLocation(idProgramShader, "textureHeight");
    glUniform1i(textureHeightLocation, textureHeight);

    GLint heightFactorLocation = glGetUniformLocation(idProgramShader, "heightFactor");
    glUniform1f(heightFactorLocation, heightFactor);
    lightPos.x=textureWidth / 2.;
    lightPos.y=100.;
    lightPos.z=textureHeight / 2.;

    GLint  lightPositionLocation = glGetUniformLocation(idProgramShader, "lightPosition");
    glUniform3fv(lightPositionLocation,1,&lightPos[0] );

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

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote

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

  string vertexShader = "shader.vert";
  string fragShader = "shader.frag";
  initShaders(idProgramShader, vertexShader, fragShader);
  glUseProgram(idProgramShader);

  glfwSetKeyCallback(win, keyCallback);
  glfwSetWindowSizeCallback(win, reshape);
    initTexture(argv[1], argv[2], &textureWidth, &textureHeight);
    glEnable(GL_DEPTH_TEST);


  // initial viewport setting
  glViewport(0,0, widthWindow, heightWindow);

  camera.position = glm::vec3(textureWidth / 2, textureWidth / 10, (-1) * (textureWidth / 4));

  cameraSetup();

  sendUniformValuesToShaders();

  meshTerrain();

  monitor = glfwGetPrimaryMonitor();
  vidmode = (glfwGetVideoMode(monitor));
  glfwGetWindowSize( win, &displayWidth, &displayHeight );

  while(!glfwWindowShouldClose(win)) {


     if (viewPortChanged){

         glViewport( 0, 0, displayWidth, displayHeight);
         viewPortChanged = false;
     }

    clearColorDepthStencilBuffers();

    camera.position += camera.gaze * camera.speed;

    cameraSetup();
    sendVertexData();


    glfwSwapBuffers(win);

    glfwPollEvents();
  }


  glfwDestroyWindow(win);
  glfwTerminate();
  return 0;
}
