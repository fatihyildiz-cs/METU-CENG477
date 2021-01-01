#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TARGET_FPS 30

GLuint program;
GLuint gTextureMap;

int hbarpos = -1;
int vbarpos = 0;
int goingForward = 1;

typedef GLubyte Pixel[3];

static const struct
{
    float x, y, z;
    float s, t;
} vertices[9] =
{
    { -2.f, 0.f, -5.f, 0.f, 1.f },
    {  0.f, 0.f, -5.f, 1.f, 1.f },
    {   -1.f,  1.f, -5.f, 0.5f, 0.f },
    { 0.f, 0.f, -5.f, 0.f, 1.f },
    {  2.f, 0.f, -5.f, 1.f, 1.f },
    {   2.f,  1.f, -5.f, 1.f, 0.f },
    {   2.f,  1.f, -5.f, 1.f, 0.f },
    { 0.f, 1.f, -5.f, 0.f, 0.f },
    {  0.f, 0.f, -5.f, 0.f, 1.f }
};

void readShader(const char* fname, char *source)
{
	FILE *fp;
	fp = fopen(fname,"r");
	if (fp==NULL)
	{
		fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
		glfwTerminate();
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
	GLuint id;
	id = glCreateShader(mode);

	glShaderSource(id,1,&source,NULL);

	glCompileShader(id);

	char error[1000];

	glGetShaderInfoLog(id,1000,NULL,error);
	printf("Compile status: \n %s\n",error);

	return id;
}

void initShaders()
{
	char *vsSource, *fsSource;
	GLuint vs,fs;

	vsSource = (char *)malloc(sizeof(char)*20000);
	fsSource = (char *)malloc(sizeof(char)*20000);

	vsSource[0]='\0';
	fsSource[0]='\0';

	program = glCreateProgram();

	readShader("texture.vs",vsSource);
	readShader("texture.fs",fsSource);

	vs = loadShader(vsSource,GL_VERTEX_SHADER);
	fs = loadShader(fsSource,GL_FRAGMENT_SHADER);

	glAttachShader(program,vs);
	glAttachShader(program,fs);

	glLinkProgram(program);

	glUseProgram(program);
	
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLuint loadTexture_ASCII(const char* tname) {
/*reads the image file assumes ppm format*/
	
	GLuint texture;
	int w,h,max;
	int Width, Height;
	int i,j;
	unsigned int r,g,b;
	int k;
	char ch;
	FILE *fp;
        Pixel *image;
	
	fp = fopen(tname,"r");
	
	printf("filename = %s\n",tname);

	/*read the header*/
	
    	fscanf(fp, "P%c\n", &ch);
	if (ch != '3') {
		fprintf(stderr, "Only ascii mode 3 channel PPM files");
		exit(-1);
	}

	/*strip comment lines*/
	ch = getc(fp);
	while (ch == '#') {
        	do {
		  ch = getc(fp);
        	}
		while (ch != '\n');

      		ch = getc(fp); 
    	}
	ungetc(ch, fp);

	/*read the width*/
	fscanf(fp,"%d",&w);
	
	/*read the height*/
	fscanf(fp,"%d",&h);
	
	/*max intensity - not used here*/
	fscanf(fp,"%d",&max);
	
	Width = w;
	Height = h;
	
	printf("Width = %d, Height = %d\n",Width,Height);
	
	image = (Pixel *)malloc(Width*Height*sizeof(Pixel));	
	
	for(i=0;i<Height;++i){
		for(j=0;j<Width;++j) {
			fscanf(fp,"%d %d %d",&r,&g,&b);
			k = i*Width+j; /*ok, can be more efficient here!*/
			(*(image+k))[0] = (GLubyte)r;
			(*(image+k))[1] = (GLubyte)g;
			(*(image+k))[2] = (GLubyte)b;
		}
	}
	fclose(fp);

    	glGenTextures(1,&texture);
    	glBindTexture(GL_TEXTURE_2D,texture);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    	glBindTexture(GL_TEXTURE_2D,0);

    	free(image);

    	return texture;
}

    
int main(void)
{
    GLFWwindow* window;

    GLuint vertex_buffer;

    GLint mvp_location, vpos_location, vtex_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(800, 400, "Texture example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
	
	printf("OpenGL version: %s - %s\n",glGetString(GL_RENDERER),glGetString(GL_VERSION));

	int error;
	if((error = glewInit()) != GLEW_OK){
		printf("Failed to initialize GLEW\n");
		printf("%s\n",glewGetErrorString(error));
		return -1;
	}
	
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    initShaders();

    glEnable(GL_TEXTURE_2D);
    gTextureMap = loadTexture_ASCII("earth.ppm");

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vtex_location = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);
    glEnableVertexAttribArray(vtex_location);
    glVertexAttribPointer(vtex_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) (sizeof(float) * 3));
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        mat4x4_identity(m);
        mat4x4_ortho(p, -2.f, 2.f, -0.5f, 1.5f, 30.f, -30.f);
        mat4x4_mul(mvp, p, m);
    	if (vbarpos>=0)
    	{
		if (goingForward)
		{
			vbarpos+=10;
			if (vbarpos>=600)
			{
				vbarpos-=10;
				goingForward=0;
			}
		}
		else
		{
			vbarpos-=10;
			if (vbarpos<=0)
			{
				vbarpos=-10;
				hbarpos=0;
				goingForward=1;
			}
		}
    	}
    	else
    	{
		if (goingForward)
		{
			hbarpos+=10;
			if (hbarpos>=600)
			{
				hbarpos-=10;
				goingForward=0;
			}
		}
		else
		{
			hbarpos-=10;
			if (hbarpos<=0)
			{
				hbarpos=-10;
				vbarpos=0;
				goingForward=1;
			}
		}
    	}

        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    	glUniform1i(glGetUniformLocation(program, "texturemap"), 0);
    	glUniform1i(glGetUniformLocation(program, "hbarpos"), hbarpos);
    	glUniform1i(glGetUniformLocation(program, "vbarpos"), vbarpos);
    	glBindTexture(GL_TEXTURE_2D,gTextureMap);
    	glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 9);
	while (glfwGetTime() < lastTime + 1.0/TARGET_FPS)
	{
	}
	lastTime += 1.0/TARGET_FPS;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
