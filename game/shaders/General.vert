#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 M;

uniform mat4 lightVP;

uniform mat4 VP;

//out vec2 UV;
//out vec4 shadowCoord;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main(){
//	UV = uv;
//	gl_Position = VP * M * vec4(vertex, 1);
//	shadowCoord = lightVP * M * vec4(vertex, 1);
//	shadowCoord /= shadowCoord.w;
//	shadowCoord = shadowCoord * 0.5 + 0.5;

	vs_out.FragPos = vec3(M * vec4(vertex, 1.0));
    vs_out.Normal = transpose(inverse(mat3(M))) * normal;
    vs_out.TexCoords = uv;
    vs_out.FragPosLightSpace = lightVP * vec4(vs_out.FragPos, 1.0);
	gl_Position = VP * M * vec4(vertex, 1);
}

