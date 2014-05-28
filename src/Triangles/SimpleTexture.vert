//shader version
#version 120

uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;                     //inverse and transpose matrix for normals
uniform mat4 modelViewProjectionMatrix;        //projectionMatrix * modelViewMatrix

//input vertex: position, normal, texture coordinates
attribute vec3 pos;
attribute vec3 nor;
attribute vec2 tex;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;

void main()
{
	gl_Position = modelViewProjectionMatrix*vec4(pos.xyz,1);

	v_position = vec3(modelViewMatrix*vec4(pos.xyz,1));
	v_normal = vec3(normalMatrix*vec4(nor.xyz,1));
	v_texcoord = vec2(tex.xy);
}