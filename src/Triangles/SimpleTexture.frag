//shader version
#version 120

//texture object
uniform sampler2D texture;

//retrieve this data from vertex shader
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;

void main()
{
    gl_FragColor = vec4(texture2D(texture, v_texcoord.xy).rgb, 1);
}