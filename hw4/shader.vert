#version 330

layout(location = 0) in vec3 vertposition;
layout(location = 1) in vec2 textcoord;
// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform mat4 normalMatrix;
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D textureSamplerForHeight;
uniform int textureWidth;
uniform int textureHeight;
uniform vec3 lightPosition;

// Output to Fragment Shader
varying vec2 textureCoord; // For texture-color
varying vec3 vertexNormal; // For Lighting computation
varying vec3 ToLightVector; // Vector from Vertex to Light;
varying vec3 ToCameraVector; // Vector from Vertex to Camera;


float getY(vec3 neighbor,float i,float j){
    vec2 neighborTexCoord;
    neighborTexCoord.x = textcoord.x-i/(textureWidth+1);
    neighborTexCoord.y = textcoord.y-j/(textureHeight+1);
    vec4 textureColor = texture(textureSamplerForHeight, neighborTexCoord);
    vec3 rgb = textureColor.xyz;
    return heightFactor*(rgb.x );
}

void main()
{

    vec3 tempPos = vertposition;

    textureCoord = textcoord;


   tempPos.y=getY(tempPos,0,0);
    // compute normal vector using also the heights of neighbor vertices
    vec2 neighborTexCoord;
    vec3 tempNormal;

    if(vertposition.x == 0 && vertposition.z == textureHeight - 1) { // For left bottom vertex
        vec3 neighbor1 = vec3(vertposition.x + 1, 0, vertposition.z);
        vec3 neighbor2 = vec3(vertposition.x + 1, 0, vertposition.z - 1);
        vec3 neighbor3 = vec3(vertposition.x , 0, vertposition.z - 1);


        neighbor1.y = getY(neighbor1,1,0);
        neighbor2.y = getY(neighbor2,1,-1);
        neighbor3.y = getY(neighbor3,0,-1);



        vec3 v1 = neighbor1 - vertposition;
        vec3 v2 = neighbor2 - vertposition;
        vec3 v3 = neighbor3 - vertposition;

        vec3 upLeftTriangleNormal = cross(v2,v1);
        vec3 bottomRightTriangleNormal = cross(v3, v2);

        tempNormal = normalize(upLeftTriangleNormal + bottomRightTriangleNormal);
    }
    else if(vertposition.x == textureWidth - 1 && vertposition.z == textureHeight - 1) {  // For right bottom vertex
        vec3 neighbor1 = vec3(vertposition.x - 1, 0, vertposition.z);
        vec3 neighbor2 = vec3(vertposition.x, 0, vertposition.z - 1);

        neighbor1.y = getY(neighbor1,-1,0);
        neighbor2.y = getY(neighbor2,0,-1);

        vec3 v1 = tempPos - neighbor1;
        vec3 v2 = neighbor2 - neighbor1;

        tempNormal = normalize(cross(v1, v2));

    }
    else if(vertposition.x == textureWidth - 1 && vertposition.z == 0) { // For top right vertex
        vec3 neighbor1 = vec3(vertposition.x - 1, vertposition.y, vertposition.z);
        vec3 neighbor2 = vec3(vertposition.x - 1, vertposition.y, vertposition.z + 1);
        vec3 neighbor3 = vec3(vertposition.x , 0, vertposition.z+1);
        neighbor1.y = getY(neighbor1,-1,0);
        neighbor2.y = getY(neighbor2,-1,1);
        neighbor3.y = getY(neighbor3,0,1);

        vec3 v1 = neighbor1 - tempPos;
        vec3 v2 = neighbor2 - tempPos;
        vec3 v3 = neighbor3 - tempPos;

        vec3 leftTriangleNormal = cross(v1,v2);
        vec3 bottomTriangleNormal = cross(v2,v3);

        tempNormal = normalize(leftTriangleNormal + bottomTriangleNormal);
    }
    else if(vertposition.x == 0 && vertposition.z == 0) { // top left vertex
        vec3 neighbor1 = vec3(vertposition.x + 1, vertposition.y, vertposition.z);
        vec3 neighbor2 = vec3(vertposition.x, vertposition.y, vertposition.z + 1);

        neighbor1.y = getY(neighbor1,1,0);
        neighbor2.y = getY(neighbor2,0,1);


        vec3 v1 = tempPos - neighbor1;
        vec3 v2 = neighbor2 - neighbor1;

        tempNormal = normalize(cross(v1, v2));
    } else if(((vertposition.z == 0 || vertposition.z == textureHeight-1)  && vertposition.x > 0) || ((vertposition.x == 0 || vertposition.x == textureWidth - 1) && vertposition.z > 0)) {
        vec3 neighbor1, neighbor2, neighbor3, neighbor4;
        if(vertposition.z == textureHeight - 1) { // Between bottom left and right vertices
            neighbor1 = vec3(vertposition.x - 1, 0 , vertposition.z);
            neighbor2 = vec3(vertposition.x , 0 , vertposition.z - 1);
            neighbor3 = vec3(vertposition.x + 1 , 0 , vertposition.z - 1);
            neighbor4 = vec3(vertposition.x + 1, 0 , vertposition.z);
            neighbor1.y = getY(neighbor1,-1,0);
            neighbor2.y = getY(neighbor2,0,-1);
            neighbor3.y = getY(neighbor3,1,-1);
            neighbor4.y = getY(neighbor4,1,0);
        } else if(vertposition.z == 0){ // Between top left and top vertices
            neighbor1 = vec3(vertposition.x + 1, 0 , vertposition.z);
            neighbor2 = vec3(vertposition.x , 0 , vertposition.z + 1);
            neighbor3 = vec3(vertposition.x - 1 , 0 , vertposition.z + 1);
            neighbor4 = vec3(vertposition.x - 1, 0 , vertposition.z);
            neighbor1.y = getY(neighbor1,1,0);
            neighbor2.y = getY(neighbor2,0,1);
            neighbor3.y = getY(neighbor3,-1,1);
            neighbor4.y = getY(neighbor4,-1,0);
        } else if(vertposition.x == 0){ // Between bottom left and top left vertices
            neighbor4 = vec3(vertposition.x, 0 , vertposition.z + 1);
            neighbor3 = vec3(vertposition.x + 1, 0 , vertposition.z);
            neighbor2 = vec3(vertposition.x + 1 , 0 , vertposition.z - 1);
            neighbor1 = vec3(vertposition.x, 0 , vertposition.z - 1);
            neighbor1.y = getY(neighbor1,0,1);
            neighbor2.y = getY(neighbor2,1,0);
            neighbor3.y = getY(neighbor3,1,-1);
            neighbor4.y = getY(neighbor4,0,-1);
        } else if(vertposition.x == textureWidth - 1) {  // Between bottom right and top right vertices
            neighbor4 = vec3(vertposition.x, 0 , vertposition.z - 1);
            neighbor3 = vec3(vertposition.x - 1, 0 , vertposition.z);
            neighbor2 = vec3(vertposition.x - 1 , 0 , vertposition.z + 1);
            neighbor1 = vec3(vertposition.x, 0 , vertposition.z + 1);
            neighbor1.y = getY(neighbor1,0,-1);
            neighbor2.y = getY(neighbor2,-1,0);
            neighbor3.y = getY(neighbor3,-1,1);
            neighbor4.y = getY(neighbor4,0,1);
        }


        vec3 v1 = neighbor1 - tempPos;
        vec3 v2 = neighbor2 - tempPos;
        vec3 v3 = neighbor3 - tempPos;
        vec3 v4 = neighbor4 - tempPos;

        vec3 leftTriangleNormal = cross(v2,v1);
        vec3 rightTopTriangleNormal = cross(v3,v2);
        vec3 rightBottomTriangleNormal = cross(v4,v3);

        tempNormal = normalize(leftTriangleNormal + rightTopTriangleNormal + rightBottomTriangleNormal);
    }
    else { // All cases in the middle i.e 6 neighbors for each vertices
        vec3 neighbor1 = vec3(vertposition.x - 1, 0 , vertposition.z);  // West of the vertex
        vec3 neighbor2 = vec3(vertposition.x - 1 , 0 , vertposition.z + 1); // Southwest of the vertex
        vec3 neighbor3 = vec3(vertposition.x , 0 , vertposition.z + 1); // South of the vertex
        vec3 neighbor4 = vec3(vertposition.x + 1 , 0 , vertposition.z); // East of the vertex
        vec3 neighbor5 = vec3(vertposition.x + 1, 0 , vertposition.z - 1); // Northeast of the vertex
        vec3 neighbor6 = vec3(vertposition.x , 0 , vertposition.z - 1); // North of the vertex

        neighbor1.y = getY(neighbor1,-1,0);
        neighbor2.y = getY(neighbor2,-1,1);
        neighbor3.y = getY(neighbor3,0,1);
        neighbor4.y = getY(neighbor4,1,0);
        neighbor5.y = getY(neighbor5,1,-1);
        neighbor6.y = getY(neighbor6,0,-1);

        vec3 v1 = neighbor1 - tempPos;
        vec3 v2 = neighbor2 - tempPos;
        vec3 v3 = neighbor3 - tempPos;
        vec3 v4 = neighbor4 - tempPos;
        vec3 v5 = neighbor5 - tempPos;
        vec3 v6 = neighbor6 - tempPos;

        vec3 t1normal = cross(v1,v2);
        vec3 t2normal = cross(v2,v3);
        vec3 t3normal = cross(v3,v4);
        vec3 t4normal = cross(v4,v5);
        vec3 t5normal = cross(v5,v6);
        vec3 t6normal = cross(v6,v1);

        tempNormal = normalize(t1normal + t2normal + t3normal + t4normal +  t5normal + t6normal);
    }

    // compute toLight vector vertex coordinate in VCS
    ToLightVector = normalize(vec3(MV*vec4(lightPosition - tempPos, 0)));
    ToCameraVector = normalize(vec3(MV*(vec4(vec3(cameraPosition) - tempPos, 0))));
    vertexNormal = normalize(vec3(normalMatrix*vec4(tempNormal,0)));


    gl_Position = MVP*vec4(tempPos,1);
    
}