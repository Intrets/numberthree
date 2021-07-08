#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D texture_t;

in vec2 UV;
in vec3 pos;

void main(){
	color = vec4(pos.z, pos.z, pos.z, 1);
}
