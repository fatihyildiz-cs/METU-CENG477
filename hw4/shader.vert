#version 330

layout(location = 0) in vec3 vertposition;
layout(location = 1) in vec2 textcoord;

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat4 invtransmatrix;
uniform vec4 cameraPosition;
uniform float heightFactor;


uniform sampler2D textureSamplerForHeight;
uniform int textureWidth;
uniform int textureHeight;
uniform vec3 lightPosition;


out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


float getY(vec3 neighbor,float i,float j){
    vec2 neighborTexCoord;
    neighborTexCoord.x = textcoord.x-i/(textureWidth+1);
    neighborTexCoord.y = textcoord.y-j/(textureHeight+1);
    vec4 textureColor = texture(textureSamplerForHeight, neighborTexCoord);
    float red = textureColor.x;
    return heightFactor*(red );
}

void main()
{



    int k1i=0,k1j=0;
    int k2i=0,k2j=0;
    int k3i=0,k3j=0;
    int k4i=0,k4j=0;
    vec3 vertpos = vertposition;
    textureCoordinate = textcoord;
    vec3 neighbor1 ,neighbor2,neighbor3,neighbor4,neighbor5,neighbor6;
    vec3 v1 ,v2,v3,v4 ,v5,v6;

    vertpos.y=getY(vertpos,0,0);

    vec2 neighborTexCoord;
    vec3 normal;
    vec3 normal1,normal2,normal3,normal4,normal5,normal6;
 if(vertposition.x == 0 ) {
     if(vertposition.z==0){
         neighbor1 = vec3(vertposition.x + 1, vertposition.y, vertposition.z);
         neighbor2 = vec3(vertposition.x, vertposition.y, vertposition.z + 1);

         neighbor1.y = getY(neighbor1, 1, 0);
         neighbor2.y = getY(neighbor2, 0, 1);

         v1 = vertpos - neighbor1;
         v2 = neighbor2 - neighbor1;

         normal = normalize(cross(v1, v2));
     }
    else if (vertposition.z == textureHeight - 1) {

     neighbor1 = vec3(vertposition.x +1, 0, vertposition.z);
     neighbor2 = vec3(vertposition.x + 1, 0, vertposition.z - 1);
     neighbor3 = vec3(vertposition.x, 0, vertposition.z - 1);

     neighbor1.y = getY(neighbor1, 1, 0);
     neighbor2.y = getY(neighbor2, 1, -1);
     neighbor3.y = getY(neighbor3, 0, -1);

     v1 = neighbor1 - vertposition;
     v2 = neighbor2 - vertposition;
     v3 = neighbor3 - vertposition;

     normal1= cross(v2, v1);
     normal2= cross(v3, v2);

     normal = normalize(normal1+normal2);
 }
}
 if(vertposition.x == textureWidth - 1 ) {
     if(vertposition.z==textureHeight-1){
            neighbor1 = vec3(vertposition.x - 1, 0, vertposition.z);
            neighbor2 = vec3(vertposition.x, 0, vertposition.z - 1);

            neighbor1.y = getY(neighbor1, -1, 0);
            neighbor2.y = getY(neighbor2, 0, -1);

            v1 = vertpos - neighbor1;
            v2 = neighbor2 - neighbor1;

            normal = normalize(cross(v1, v2));

     }


     else if ( vertposition.z == 0) {
           neighbor1 = vec3(vertposition.x - 1, vertposition.y, vertposition.z);
           neighbor2 = vec3(vertposition.x - 1, vertposition.y, vertposition.z + 1);
           neighbor3 = vec3(vertposition.x, 0, vertposition.z+1);
           neighbor1.y = getY(neighbor1, -1, 0);
           neighbor2.y = getY(neighbor2, -1, 1);
           neighbor3.y = getY(neighbor3, 0, 1);
           v1 = neighbor1 - vertpos;
           v2 = neighbor2 - vertpos;
           v3 = neighbor3 - vertpos;

           normal1 = cross(v1, v2);
           normal2= cross(v2, v3);

           normal = normalize(normal1+normal2);
     }
}

    else if(((vertposition.z == 0 || vertposition.z == textureHeight-1)  && vertposition.x > 0) || ((vertposition.x == 0 || vertposition.x == textureWidth - 1) && vertposition.z > 0)) {

        if(vertposition.z == textureHeight - 1) {

            k1i=-1;
            k1j=0;
            k2i=0;
            k2j=-1;
            k3i=1;
            k3j=-1;
            k4i=1;
            k4j=0;

        } else if(vertposition.z == 0){
           k1i=1;
            k1j=0;
            k2i=0;
            k2j=1;
            k3i=-1;
            k3j=1;
            k4i=-1;
            k4j=0;
        } else if(vertposition.x == 0){

            k1i=0;
            k1j=1;
            k2i=1;
            k2j=0;
            k3i=1;
            k3j=-1;
            k4i=0;
            k4j=-1;
        } else if(vertposition.x == textureWidth - 1) {

            k1i=0;
            k1j=-1;
            k2i=-1;
            k2j=0;
            k3i=-1;
            k3j=1;
            k4i=0;
            k4j=1;
        }
        neighbor1 = vec3(vertposition.x+k1i, 0 , vertposition.z +k1j);
        neighbor2 = vec3(vertposition.x +k2i, 0 , vertposition.z+k2j);
        neighbor3 = vec3(vertposition.x +k3i , 0 , vertposition.z + k3j);
        neighbor4 = vec3(vertposition.x+k4i, 0 , vertposition.z + k4j);
        neighbor1.y = getY(neighbor1,k1i,k1j);
        neighbor2.y = getY(neighbor2,k2i,k2j);
        neighbor3.y = getY(neighbor3,k3i,k3j);
        neighbor4.y = getY(neighbor4,k4i,k4j);


        v1 = neighbor1 - vertpos;
        v2 = neighbor2 - vertpos;
        v3 = neighbor3 - vertpos;
        v4 = neighbor4 - vertpos;

        normal1 = cross(v2,v1);
        normal2 = cross(v3,v2);
        normal3 = cross(v4,v3);

        normal = normalize(normal1+normal2+normal3);
    }
    else {
        neighbor1 = vec3(vertposition.x - 1, 0 , vertposition.z);
        neighbor2 = vec3(vertposition.x - 1 , 0 , vertposition.z + 1);
        neighbor3 = vec3(vertposition.x , 0 , vertposition.z + 1);
        neighbor4 = vec3(vertposition.x + 1 , 0 , vertposition.z);
        neighbor5 = vec3(vertposition.x + 1, 0 , vertposition.z - 1);
        neighbor6 = vec3(vertposition.x , 0 , vertposition.z - 1);

        neighbor1.y = getY(neighbor1,-1,0);
        neighbor2.y = getY(neighbor2,-1,1);
        neighbor3.y = getY(neighbor3,0,1);
        neighbor4.y = getY(neighbor4,1,0);
        neighbor5.y = getY(neighbor5,1,-1);
        neighbor6.y = getY(neighbor6,0,-1);

        v1 = neighbor1 - vertpos;
        v2 = neighbor2- vertpos;
        v3 = neighbor3 - vertpos;
        v4 = neighbor4 - vertpos;
        v5 = neighbor5 - vertpos;
        v6 = neighbor6 - vertpos;

        normal1 = cross(v1,v2);
        normal2 = cross(v2,v3);
        normal3 = cross(v3,v4);
        normal4 = cross(v4,v5);
        normal5 = cross(v5,v6);
        normal6 = cross(v6,v1);


       normal = normalize(normal1 + normal2 + normal3 + normal4 +  normal5 + normal6);

    }




    ToLightVector = normalize(vec3(modelView*vec4(lightPosition - vertpos, 0)));
    ToCameraVector = normalize(vec3(modelView*(vec4(vec3(cameraPosition) - vertpos, 0))));
    vertexNormal = normalize(vec3(invtransmatrix*vec4(normal,0)));


    gl_Position = modelViewProjection*vec4(vertpos,1);
    
}