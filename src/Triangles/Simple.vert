//shader version
#version 120

//projectionMatrix*modelViewMatrix
uniform mat4 modelViewProjectionMatrix;

attribute vec3 pos;

void main()
{
	gl_Position = modelViewProjectionMatrix*vec4(pos.xyz,1);
}