#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in mat4 M;

uniform mat4 lightVP;
uniform mat4 VP;

out VS_OUT {
    vec3 fragWorldPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragLightPos;
} vs_out;

void main(){
	vec4 fragWorldPos4 = M * vec4(vertex, 1.0);
	vs_out.fragWorldPos = fragWorldPos4.xyz;
    vs_out.normal = transpose(inverse(mat3(M))) * normal;
    vs_out.texCoords = uv;
    vs_out.fragLightPos = lightVP * fragWorldPos4;
	gl_Position = VP * fragWorldPos4;
}
