#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 MVP;

out vec2 UV;
out vec3 pos;

void main(){
	UV = uv;
	gl_Position = MVP * vec4(vertex, 1);
	pos = gl_Position.xyz;
}

