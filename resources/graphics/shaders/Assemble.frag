#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D albedo_t;
uniform sampler2D lighting_t;

in vec2 UV;

void main(){
	color = texture(albedo_t, UV) * texture(lighting_t, UV).x;
//	color = vec4(texture(lighting_t, UV).x);
//	color = vec4(texture(albedo_t, UV));
//	color = texture(albedo_t, UV);


	color.w = 1.0;
}