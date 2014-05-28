//shader version
#version 120

//texture object
uniform sampler2D texture;

// the properties of the material
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform float a0;
uniform float a1;
uniform float a2;

uniform vec3 lightViewPosition; // light position in camera coordinates


//retrieve this data from vertex shader
varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_texcoord;
varying vec3 v;                        // direction in camera coordinates towards viewer (interpolated)
varying vec3 l;                        // direction in camera coordinates towards light (interpolated)

void main()
{
    // we have only one light source --- the sun, so let's cheat and define its properties right here
    vec3 lightAmbient = vec3(1, 1, 1);
    vec3 lightDiffuse = vec3(1, 1, 1);
    vec3 lightSpecular = vec3(1, 1, 1);

    vec3 ambient = materialAmbient * lightAmbient * texture2D(texture, v_texcoord.xy).rgb;
    vec3 diffuse = materialDiffuse * lightDiffuse * texture2D(texture, v_texcoord.xy).rgb;
	vec3 specular = materialSpecular * lightSpecular;

    vec3 color = ambient;

    vec3 N = normalize(v_normal);
    vec3 L = normalize(l);
    float angle = max(dot(N, L), 0);
    if (angle > 0) {
        float distance = length(lightViewPosition - v_position);
        vec3 H = normalize(normalize(v) + L);
        color += (diffuse * angle + specular * pow(max(dot(H, N), 0), materialShininess)) / 
                 (a0 + a1 * distance + a2 * (distance * distance));
    }

	gl_FragColor = vec4(color, 1);
}