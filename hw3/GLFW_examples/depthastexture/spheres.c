#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <sys/time.h>

typedef struct {
        float x; float y; float z;
} vec3;

typedef struct {
	float r; float g; float b;
} color;

typedef struct {
        color c;
        vec3 center;
        float radius;
        float speed;
        float e;
} sphere;

struct timeval t1, t2;

sphere spheres[100];
int numSpheres = 0;

float camRadius;
float camHeight;
float camAngle;

int cam = 1;
int ss = -1;

float gravity = 10;

GLint t;
GLuint simpleProgram;
GLuint gProgram;
GLuint FBO; // frame buffer object

unsigned int renderTexture,depthTexture;


void initSpheres(int n)
{
     int i;
     numSpheres = n;
     srand(time(NULL));
     for (i=0;i<n;i++)
     {
         spheres[i].c.r = (rand()%255)/255.0;
         spheres[i].c.g = (rand()%255)/255.0;
         spheres[i].c.b = (rand()%255)/255.0;
         
         spheres[i].center.x = -100+(rand()%200);
         spheres[i].center.y = 50+(rand()%50);
         spheres[i].center.z = -100+(rand()%200);
         
         spheres[i].radius = 2+(rand()%10);
         spheres[i].e = 0.7+((rand()%100)/300.0);
         spheres[i].speed = 0;
     }
}

unsigned int createTexture(int w,int h, char isDepth)
{
	unsigned int textureId;
	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
	glTexImage2D(GL_TEXTURE_2D,0,(!isDepth ? GL_RGBA8 : GL_DEPTH_COMPONENT),w,h,0,isDepth ? GL_DEPTH_COMPONENT : GL_RGBA,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	
	int i;
	i=glGetError();
	if(i!=0)
	{
		printf("Error happened while loading the texture: %d\n",i);
	}
    glBindTexture(GL_TEXTURE_2D,0);
	return textureId;
}

/// Print out the information log for a shader object 
/// @arg obj handle for a program object
static void printProgramInfoLog(GLuint obj)
{
	GLchar infoLog[500];
	GLint charsWritten = 0;
	GLint infologLength = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
	printf("%s\n",infoLog);
}

void readShader(const char *fname, char *source)
{
     FILE *fp;
     fp = fopen(fname,"r");
     if (fp==NULL)
     {
         fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
         exit(1);
     }
     char tmp[300];
     while (fgets(tmp,300,fp)!=NULL)
     {
           strcat(source,tmp);
           strcat(source,"\n");
     }
}

unsigned int loadShader(const char *source, unsigned int mode)
{
         unsigned int id;
         id = glCreateShader(mode);
                  
         glShaderSource(id,1,&source,NULL);
         
         glCompileShader(id);
         
         char error[1000];
         
         glGetShaderInfoLog(id,1000,NULL,error);
         printf("Compile status: \n %s\n",error);
         
         return id;         
}


void initSimpleShader()
{
    char *vsSource,*fsSource;
    GLuint vs,fs;

    vsSource = (char *)malloc(sizeof(char)*20000);
    fsSource = (char *)malloc(sizeof(char)*20000);

    vsSource[0]='\0';
    fsSource[0]='\0';
    
    simpleProgram = glCreateProgram();

    readShader("simple.vs",vsSource);
    readShader("simple.frag",fsSource);
    
    //printf("%s",vsSource);
    //printf("%s",fsSource);

    vs = loadShader(vsSource,GL_VERTEX_SHADER);
    fs = loadShader(fsSource,GL_FRAGMENT_SHADER);
    printProgramInfoLog(simpleProgram);   // verifies if all this is ok so far

    glAttachShader(simpleProgram, vs);
    glAttachShader(simpleProgram, fs);
    printProgramInfoLog(simpleProgram);   // verifies if all this is ok so far

    glLinkProgram(simpleProgram);
    printProgramInfoLog(simpleProgram);   // verifies if all this is ok so far

}

void initShaders()
{
    char *vsSource,*fsSource;
    GLuint vs,fs;

    vsSource = (char *)malloc(sizeof(char)*20000);
    fsSource = (char *)malloc(sizeof(char)*20000);

    vsSource[0]='\0';
    fsSource[0]='\0';
    
    gProgram = glCreateProgram();

    readShader("depthShader.vs",vsSource);
    readShader("depthShader.frag",fsSource);
    
    //printf("%s",vsSource);
    //printf("%s",fsSource);

    vs = loadShader(vsSource,GL_VERTEX_SHADER);
    fs = loadShader(fsSource,GL_FRAGMENT_SHADER);
    printProgramInfoLog(gProgram);   // verifies if all this is ok so far

    glAttachShader(gProgram, vs);
    glAttachShader(gProgram, fs);
    printProgramInfoLog(gProgram);   // verifies if all this is ok so far

    glLinkProgram(gProgram);
    printProgramInfoLog(gProgram);   // verifies if all this is ok so far
    glUseProgram(gProgram);
    printProgramInfoLog(gProgram);   // verifies if all this is ok so far
}

void initScene() {

     int i;
    glClearColor(1.0,1.0,1.0,1.0);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
  	glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	
	renderTexture=createTexture(800,800,0);
	depthTexture=createTexture(800,800,1);
	
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	//GL_COLOR_ATTACHMENT0
	//GL_DEPTH_ATTACHMENT
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,renderTexture,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);

	i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer is not OK, status=%d\n",i);
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,1,10,500);
    glMatrixMode(GL_MODELVIEW);
    initSpheres(20);
    camRadius = 300;
    camHeight = 100;
    camAngle = 270;

	initShaders();


    gettimeofday(&t1,NULL);

}


void getDepthImage()
{
    int i,m;
    float camx,camy,camz;

    glUseProgram(simpleProgram);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,renderTexture,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW); // switch to modelview matrix for viewing and transformation process.

    glLoadIdentity();

    camx = camRadius*cos(camAngle*M_PI/180.0);
    camy = camHeight;
    camz = -camRadius*sin(camAngle*M_PI/180.0);
   

	/*
   if (cam == 1) 
    	gluLookAt(camx,camy,camz,0,0,0,0,1,0);
    else
		gluLookAt(spheres[ss].center.x,spheres[ss].center.y+spheres[ss].radius+1,spheres[ss].center.z,1,spheres[ss].center.y+spheres[ss].radius+1,1,0,1,0);
	
	*/
   	gluLookAt(200,200,200,0,0,0,0,1,0);

    for (i=0;i<numSpheres;i++)
    {
        glPushMatrix();
        glTranslatef(spheres[i].center.x,spheres[i].center.y,spheres[i].center.z);
		GLUquadricObj *quadric;
		quadric = gluNewQuadric();

		gluQuadricDrawStyle(quadric, GLU_FILL );
		gluSphere( quadric , spheres[i].radius,40,40);
        glPopMatrix();
    }
    glBegin(GL_TRIANGLES);
	    glVertex3f(-150,0,150);
        glVertex3f(150,0,-150);
       	glVertex3f(-150,0,-150);
	    glVertex3f(-150,0,150);
	    glVertex3f(150,0,150);
	    glVertex3f(150,0,-150);
     glEnd();
 	
    glBindFramebuffer(GL_FRAMEBUFFER,0);

	glUseProgram(0);

}

void renderScene() {
     
    int i,j;
    float camx,camy,camz;
    float lightPos1[] = {200,200,200,0.0};
    float lightPos2[] = {-200,200,-200,0.0};
    float white[]= {1,1,1,1};


	getDepthImage();

    glUseProgram(gProgram);
    glColor3f(0,0,0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    
 	glMatrixMode(GL_MODELVIEW);     
    glLoadIdentity();

    camx = camRadius*cos(camAngle*M_PI/180.0);
    camy = camHeight;
    camz = -camRadius*sin(camAngle*M_PI/180.0);
   
    
	if (cam == 1) 
    	gluLookAt(camx,camy,camz,0,0,0,0,1,0);
    else
	    gluLookAt(spheres[ss].center.x,spheres[ss].center.y+spheres[ss].radius+1,spheres[ss].center.z,1,spheres[ss].center.y+spheres[ss].radius+1,1,0,1,0);
	
	
	//gluLookAt(200,200,200,0,0,0,0,1,0);


    glLightfv(GL_LIGHT0,GL_POSITION,lightPos1);
    glLightfv(GL_LIGHT1,GL_POSITION,lightPos2);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,white);

     
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,depthTexture);
        glUniform1i(glGetUniformLocation(gProgram,"depth"),0);

        glBegin(GL_QUADS);
                glTexCoord2f(0,1);              //gl_MultiTexCoord0
                glVertex3f(-100,100,-4);    //gl_Vertex
                glTexCoord2f(0,0);
                glVertex3f(-100,-100,-4);
                glTexCoord2f(1,0);
                glVertex3f(100,-100,-4);
                glTexCoord2f(1,1);
                glVertex3f(100,100,-4);
        glEnd();

}

void animate()
{
     int i;
     float seconds;
     long secs, microsecs;
     gettimeofday(&t2,NULL); 
     secs=(t2.tv_sec - t1.tv_sec);
     microsecs = ((secs*1000000) + t2.tv_usec) - (t1.tv_usec);
     seconds = microsecs/1000000.0;

     seconds*=3;

     for (i=0;i<numSpheres;i++)
     {
          spheres[i].speed += -gravity*seconds;
	  spheres[i].center.y += spheres[i].speed*seconds - 0.5*gravity*(seconds*seconds);

	  if (spheres[i].speed<0 && spheres[i].center.y-spheres[i].radius<=0)
      { spheres[i].speed = -1*spheres[i].speed*spheres[i].e;
        spheres[i].center.y = spheres[i].radius; }
        }
     camAngle+=0.1;
     if (camAngle>=360) camAngle = 0;         
     gettimeofday(&t1,NULL); 
              
}

/*-------------------
Keyboard
-------------------*/
static void processNormalKeys(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	if (key == 'r' || key == 'R')
	{
		initSpheres(20);
		return;
	}
	if (key == 'q' || key == 'Q')
	{
            camHeight+=5.0;
    }
	if (key == 'a' || key == 'A')
	{
            camHeight-=5.0;
    }
	if (key == 'p' || key == 'P')
	{
            camRadius+=5.0;
    }
	if (key == 'l' || key == 'L')
	{
            camRadius-=5.0;
    }
        if (key == 'c' || key == 'C')
	{
	    cam = 1;
	    ss = -1;
        }
       if (key == '0')
	{
		cam = 0;
		ss = 0;
	}
       if (key == '1')
	{
		cam = 0;
		ss = 1;
	}
       if (key == '2')
	{
		cam = 0;
		ss = 2;
	}
       if (key == '3')
	{
		cam = 0;
		ss = 3;
	}
       if (key == '4')
	{
		cam = 0;
		ss = 4;
	}
       if (key == '5')
	{
		cam = 0;
		ss = 5;
	}
       if (key == '6')
	{
		cam = 0;
		ss = 6;
	}
       if (key == '7')
	{
		cam = 0;
		ss = 7;
	}

}

double lastTime;
int nbFrames;

void showFPS(GLFWwindow *pWindow)
{
    // Measure speed
     double currentTime = glfwGetTime();
     double delta = currentTime - lastTime;
	 char ss[500] = {};
     nbFrames++;
     if ( delta >= 1.0 ){ // If last cout was more than 1 sec ago
         //cout << 1000.0/double(nbFrames) << endl;

         double fps = ((double)(nbFrames)) / delta;

         sprintf(ss,"Spheres. %lf FPS",fps);

         glfwSetWindowTitle(pWindow, ss);

         nbFrames = 0;
         lastTime = currentTime;
     }
}

int main(int argc, char **argv)
{

        
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
 
    if (!glfwInit())
	{
		printf("Failed to initialize GLFW\n" );
        exit(EXIT_FAILURE);
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
 
    window = glfwCreateWindow(800, 800, "Spheres", NULL, NULL);

    if (!window)
    {
		printf("Failed to open GLFW window.");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
	glfwMakeContextCurrent(window);

	printf("OpenGL version: %s - %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));

    printf("GLSL Version: %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));

	int error;
	if((error = glewInit()) != GLEW_OK){
		printf("Failed to initialize GLEW\n");
		printf("%s\n",glewGetErrorString(error));
		return -1;
	}

    glfwSetKeyCallback(window, processNormalKeys);
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
 
    initScene();

    lastTime = glfwGetTime();
	nbFrames = 0;
    while (!glfwWindowShouldClose(window))
    {

        renderScene();
        animate();
 
		showFPS(window);
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

