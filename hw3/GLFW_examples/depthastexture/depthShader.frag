#version 120

uniform sampler2D depth;
varying vec2 texcoord;

void main()
{
	vec4 texcolor = texture2D(depth,texcoord);
	float d = texcolor.r-0.96;
	d = d*(1.0/0.06);
//	d = 1-d;
	gl_FragColor = vec4(d,d,d,1.0);
}
