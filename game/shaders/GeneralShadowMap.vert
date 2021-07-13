#version 330 core

layout(location = 0) in vec3 vertex;

layout(location = 1) in mat4 M;

uniform mat4 VP;


void main(){
	gl_Position = VP * M * vec4(vertex, 1);
}

