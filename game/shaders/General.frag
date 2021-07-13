#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D texture_t;
uniform sampler2D shadowMap_t;

//in vec2 UV;
//in vec4 shadowCoord;
//
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 lightPos;

void main(){
	float vis = 0.0;
	float bias = 0.000005;

	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

	vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//	bias /= 9.0;
	if (texture(shadowMap_t, projCoords.xy).x < (projCoords.z - bias)){
		vis = 1.0;
	}

    float diffuse = max(dot(lightDir, normal), 0.0);

	float ambient = 0.5;

    float lighting = (ambient + (1.0 - vis) * (diffuse));
	color = texture(texture_t, fs_in.TexCoords) * vec4(lighting, lighting, lighting, 1.0);


}
