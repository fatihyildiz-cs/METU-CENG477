#version 120

uniform vec4 lightPosition;
uniform int sensor;
uniform mat4 sensorMVP;
varying vec3 Norm;
varying vec4 position;
varying vec3 toLight;
varying vec4 tColor;
varying vec4 sensorVertexPos;

void main()
{
    vec4 green = vec4(0.2,0.8,0.3,1.0);
    float h = gl_Vertex.y;
    // 80, 50, 100 works fine
    float r = h/80.0;
    float g = h/50.0;
    float b = h/100.0;
    if (r>1.0) r = 1.0;
    if (g>1.0) g = 1.0;
    if (b>1.0) b = 1.0;
    tColor = vec4(r,g,b,1.0);
    if (sensor==1) tColor = vec4(1.0,0.3,0.2,1.0);
    
    sensorVertexPos = sensorMVP * gl_Vertex;
    Norm = gl_NormalMatrix*gl_Normal;

    toLight = vec3(lightPosition - (gl_ModelViewMatrix * gl_Vertex));
    
    position = gl_ModelViewMatrix * gl_Vertex;
    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
}
