#version 120
uniform mat4 MVP;

attribute vec2 vPos;
attribute vec2 vTexCoord;

varying vec2 texture_coordinate;

void main()
{
	gl_Position = MVP*vec4(vPos, 0.0, 1.0);

	texture_coordinate = vTexCoord;
}
