#version 120

varying vec2 texture_coordinate;

uniform sampler2D texturemap;
uniform int hbarpos;
uniform int vbarpos;

void main()
{
   int dist;
	
    vec4 cx = texture2D(texturemap, texture_coordinate);

    if (vbarpos>=0)
	dist = int(abs(floor(texture_coordinate.x*600)-vbarpos));
    else
	dist = int(abs(floor(texture_coordinate.y*600)-hbarpos));

    	gl_FragColor = pow((dist/600.0),0.1)*cx+pow(((600-dist)/600.0),10.0)*vec4(1,0,0,0);

}
