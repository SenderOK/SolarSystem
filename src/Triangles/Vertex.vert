//shader version
#version 120

uniform mat4 modelViewMatrix;
uniform mat4 normalMatrix;                     //inverse and transpose matrix for normals
uniform mat4 modelViewProjectionMatrix;        //projectionMatrix * modelViewMatrix
uniform mat4 viewMatrix;                       

uniform vec3 lightPosition;                    //position of the sun in world coordinates

//input vertex: position, normal, texture coordinates
attribute vec3 pos;
attribute vec3 nor;
attribute vec2 tex;

//output vertex to fragment shader
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v;                        // direction in camera coordinates towards viewer (to be interpolated)
varying vec3 l;                        // direction in camera coordinates towards light (to be interpolated)

void main()
{
	gl_Position = modelViewProjectionMatrix*vec4(pos.xyz,1);

	v_position = vec3(modelViewMatrix*vec4(pos.xyz,1));
	v_normal = vec3(normalMatrix*vec4(nor.xyz,1));
	v_texcoord = vec2(tex.xy);

    l = (viewMatrix * vec4(lightPosition, 1)).xyz - v_position;
    v = vec3(0, 0, 0) - v_position;
}