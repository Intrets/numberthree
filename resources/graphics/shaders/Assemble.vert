#version 330 core

layout(location = 0) in vec2 vertex;

out vec2 UV;

void main(){
//	vec2 v = vertex;
//	gl_Position = vec4(vertex * worldTarget.zw + worldTarget.xy + offset, depth, 1);
	gl_Position = vec4(vertex*2 - 1, 0, 1);
	UV = vertex;
}

