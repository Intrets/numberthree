#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D texture_t;
uniform float time;

in vec2 UV;
in vec3 pos;

void main(){
	float r = (1 + sin((length(pos) * 1.0f + time)*0.01))/2.0;
	float g = (1 + cos((length(pos) * 0.7f + time*2.3)*0.02))/2.0;
	float b = (1 + cos((length(pos) * 0.3f + time*3.1)*0.03))/2.0;
//	vec3 pos2 = vec3(sin(l*100.0));
//	color = vec4(pos2.z, pos2.z, pos2.z, 1);
	color = vec4(r, g, b, 1);
//	float l = length(pos) / 3000.0;
//
//	color += l;
//	color.a = 1;
//
}
