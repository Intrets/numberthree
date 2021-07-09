#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 M;

uniform mat4 VP;
uniform vec3 camPos;

out vec2 UV;
out vec3 pos;

void main(){
	UV = uv;
	pos = (M * vec4(vertex, 1)).xyz - camPos;
	pos = pos * 10.0;
	gl_Position = VP * M * vec4(vertex, 1);
}

