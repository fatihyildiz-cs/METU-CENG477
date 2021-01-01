#version 120
varying vec3 toLight;
varying vec3 Norm;
varying vec4 position;
varying vec4 tColor;

uniform sampler2D depthMap;

varying vec4 sensorVertexPos;

void main()
{
    float seen = 0.0;
    vec4 rColor;
    
    vec4 sensorVertexPos2 = sensorVertexPos;
    
    sensorVertexPos2/=sensorVertexPos2.w;
    
    vec3 norm = normalize(Norm);
    
    if (texture2D(depthMap,sensorVertexPos2.xy).r>=sensorVertexPos2.z-0.0001)
        seen=1.0;
    
    vec4 green = vec4(0.2,0.8,0.3,1.0);
    vec4 red = vec4(0.8,0.3,0.2,1.0);
    vec4 gray = vec4(0.3, 0.3, 0.3, 1.0);
    vec4 ka = vec4(0.25,0.25,0.25,1.0);
    vec4 Ia = vec4(0.3,0.3,0.3,1.0);

    // Light vector
    vec3 lightv = normalize(toLight);
    vec3 viewv = -normalize(position.xyz).xyz;
    vec3 halfv = normalize(lightv + viewv);
    
    if (seen>0.5) rColor = red;
    else rColor = tColor;
    
    /* diffuse reflection */
    vec4 diffuse = max(0.0, dot(lightv, norm))*rColor;
    
    /* ambient reflection */
    
    vec4 ambient = ka*Ia;
    
    /* specular reflection */
    vec4 specular = vec4(0.0, 0.0, 0.0, 1.0);
    specular = pow(max(0.0, dot(norm, halfv)),5)*gray;
    vec3 color = clamp(0.8*vec3(ambient + diffuse + specular), 0.0, 1.0);
    
    gl_FragColor = vec4(color, 1.0);
}
