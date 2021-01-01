#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using parser::Vec3f;
using namespace std;
//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow* win = NULL;

static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void displayScene(){

    bool firstTime = true;

    static int vertexPosDataSizeInBytes;

    if(firstTime){

        firstTime = false;

        vertexPosDataSizeInBytes = scene.vertex_data.size() * 3 * sizeof(GLfloat);
        int indexDataSizeInBytes = scene.numberOfFaces * 3 * sizeof(GLuint);
        GLuint* indices = new GLuint[scene.numberOfFaces * 3];

        GLuint vertexAttribBuffer, indexBuffer;

        glGenBuffers(1, &vertexAttribBuffer);
        glGenBuffers(1, &indexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glBufferData(GL_ARRAY_BUFFER, 2*vertexPosDataSizeInBytes, 0, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indices, GL_STATIC_DRAW);

    }
    
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glNormalPointer(GL_FLOAT, 0, (const void*)vertexPosDataSizeInBytes);

}

void clearAndDisplay(){

    glClearColor(scene.background_color.x, scene.background_color.y,
               scene.background_color.z, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    displayScene();
    // displayOneTriangle();
}

void displayOneTriangle() {
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0); // color state is red
    glVertex3f(0, 0.5, 0); // first vertex
    glColor3f(0.0, 1.0, 0.0); // color state is green
    glVertex3f(-0.5, -0.5, 0); // second vertex
    glColor3f(0.0, 0.0, 1.0); //color state is blue
    glVertex3f(0.5, -0.5, 0); // third vertex
    glEnd();
}

void initializeDisplay(){
    
    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
}

void adjustCamera(){

    const parser::Camera camera = scene.camera;
    const Vec3f eye = camera.position;
    const Vec3f center = eye + camera.gaze * camera.near_distance;
    const Vec3f v = camera.up;
    const parser::Vec4f near_plane = camera.near_plane;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(near_plane.x, near_plane.y, near_plane.z, near_plane.w,
                camera.near_distance, camera.far_distance);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, v.x, v.y, v.z);
}

void adjustLights(){

    const GLfloat ambient[4] = {scene.ambient_light.x, scene.ambient_light.y,
                              scene.ambient_light.z, 1.};
    const std::vector<parser::PointLight> point_lights = scene.point_lights;
    cout << "there are " << point_lights.size() << " lights" << endl;
    for (size_t i = 0; i < point_lights.size(); i++) {
        const parser::PointLight light = point_lights[i];
        const GLfloat intensity[4] = {light.intensity.x, light.intensity.y,
                                    light.intensity.z, 1.};
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, intensity);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, intensity);
        glEnable(GL_LIGHT0 + i);
    }
}

int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    initializeDisplay();
    adjustLights();
    adjustCamera();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(win)) {
        

        clearAndDisplay();
        
        glfwSwapBuffers(win);
        glfwWaitEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
