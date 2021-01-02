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

    static bool firstTime = true;

    static int vertexPosDataSizeInBytes;

    if(firstTime){

        firstTime = false;
        vertexPosDataSizeInBytes = scene.vertex_data.size() * 3 * sizeof(GLfloat);
        int indexDataSizeInBytes = scene.numberOfFaces * 3 * sizeof(GLuint);
        GLuint* indices = scene.indices;
        GLfloat *vertexPos=scene.vertexPos;
        GLfloat  * normals=scene.normals;
        GLuint vertexAttribBuffer, indexBuffer;

        glGenBuffers(1, &vertexAttribBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
        glBufferData(GL_ARRAY_BUFFER, 2*vertexPosDataSizeInBytes, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexPosDataSizeInBytes, vertexPos);
        glBufferSubData(GL_ARRAY_BUFFER,vertexPosDataSizeInBytes,vertexPosDataSizeInBytes,normals);
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indices, GL_STATIC_DRAW);

    }

    glVertexPointer(3, GL_FLOAT, 0, 0);
    glNormalPointer(GL_FLOAT, 0, (const void*)vertexPosDataSizeInBytes);
    int offset=0;
    for(const auto & mesh : scene.meshes){

        const auto material = scene.materials[mesh.material_id];

        GLfloat ambColor[4] = {material.ambient.x, material.ambient.y, material.ambient.z, 1.0};
        GLfloat diffColor[4] = {material.diffuse.x, material.diffuse.y, material.diffuse.z, 1.0};
        GLfloat specColor[4] = {material.specular.x, material.specular.y, material.specular.z, 1.0};
        GLfloat specExp [ 1 ] = {material.phong_exponent};

        glMaterialfv ( GL_FRONT , GL_AMBIENT , ambColor );
        glMaterialfv ( GL_FRONT , GL_DIFFUSE , diffColor );
        glMaterialfv ( GL_FRONT , GL_SPECULAR , specColor );
        glMaterialfv ( GL_FRONT , GL_SHININESS , specExp );



        if(mesh.mesh_type == "Wireframe"){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glDrawElements(GL_TRIANGLES, mesh.faces.size()*3, GL_UNSIGNED_INT, (const void* )(offset*3*sizeof(GLuint)));

        offset = offset + mesh.faces.size();
        
    }
    

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

void initializeDisplay(){
    
    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    if(scene.culling_enabled == 1){

        glEnable(GL_CULL_FACE);

        if(scene.culling_face == 0){
            glCullFace(GL_BACK);
        }
        else{
            glCullFace(GL_FRONT);
        }
    }
    
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
