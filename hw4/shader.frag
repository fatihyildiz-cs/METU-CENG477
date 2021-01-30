#version 330

uniform int textureWidth;
uniform int textureHeight;
uniform vec4 cameraPosition;
uniform sampler2D textureSamplerForColor;




in vec2 textureCoordinate;
in vec3 vertexNormal; // For Lighting computation
in vec3 ToLightVector; // Vector from Vertex to Light;
in vec3 ToCameraVector;

void main() {


  vec4 textureColor = texture(textureSamplerForColor, textureCoordinate);


  vec4 ka = vec4(0.25,0.25,0.25,1.0);
  vec4 Ia = vec4(0.3,0.3,0.3,1.0);
  vec4 Id = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 kd = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 Is = vec4(1.0, 1.0, 1.0, 1.0);
  vec4 ks = vec4(1.0, 1.0, 1.0, 1.0);
  int specExp = 100;

  float aci1 = clamp(dot(vertexNormal, ToLightVector),0,1);
  vec3 halfvec = normalize(ToLightVector + ToCameraVector);
  float aci2 = clamp(dot(vertexNormal, halfvec),0,1);
  vec4 ambient = ka * Ia;
  vec4 diffuse = kd * aci1 * Id;
  vec4 specular = ks * pow(aci2,specExp) * Is;
   gl_FragColor = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);


}