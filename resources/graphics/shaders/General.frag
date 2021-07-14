#version 330 core

layout(location = 0) out vec4 color;

uniform sampler2D texture_t;
uniform sampler2D shadowMap_t;

uniform vec3 viewPos;

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

vec2 poissonDisk[20] = vec2[](
vec2(0.00000, 0.00000),
vec2(-0.06964, -0.30883),
vec2(0.20936, -0.45704),
vec2(0.11959, -0.29279),
vec2(-0.00575, 0.00055),
vec2(-0.04620, -0.31342),
vec2(-0.00170, 0.00007),
vec2(0.31144, 0.00282),
vec2(0.00503, -0.00248),
vec2(-0.00014, -0.00067),
vec2(0.02711, -0.00632),
vec2(-0.12313, -0.49396),
vec2(0.00005, 0.00039),
vec2(-0.29069, -0.11311),
vec2(-0.39065, -0.66148),
vec2(0.31935, -0.12934),
vec2(0.02307, -0.00557),
vec2(-0.49534, -0.36279),
vec2(-0.00366, -0.00870),
vec2(0.00089, -0.00008)
);

vec2 circle[17] = vec2[](
vec2(0.36124, 0.93247),
vec2(0.67370, 0.73901),
vec2(0.89516, 0.44574),
vec2(0.99573, 0.09227),
vec2(0.96183, -0.27366),
vec2(0.79802, -0.60263),
vec2(0.52643, -0.85022),
vec2(0.18375, -0.98297),
vec2(-0.18375, -0.98297),
vec2(-0.52643, -0.85022),
vec2(-0.79802, -0.60263),
vec2(-0.96183, -0.27366),
vec2(-0.99573, 0.09227),
vec2(-0.89516, 0.44574),
vec2(-0.67370, 0.73901),
vec2(-0.36124, 0.93247),
vec2(-0.00000, 1.00000)
);


void main(){
	float vis = 0.0;
	float bias = 0.000007;

	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

	vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float p = 1 / (1 - projCoords.z);
	float pp = 1 / (1 - texture(shadowMap_t, projCoords.xy).x);

	float prox = 100.0;

	float m = p;
	vec2 vm = vec2(0);
	float n = pp;
	vec2 vn = vec2(0);
	for (int i = 0; i < 17; i++) {
		float f = 1 / (1 - texture(shadowMap_t, projCoords.xy + circle[i] / prox).x);

		if (f < m) {
			m = f;
			vm = circle[i] / prox;
		}

		if (f > n) {
			n = f;
			vn = circle[i] / prox;
		}
	}

	float sshadow = 1.0;

	if (m != p) {
		vec2 v = projCoords.xy + vm/2;
		float find = (projCoords.z + (1 - 1/m))/2;
		vm /= 2;
		for (int i = 0; i < 10; i++){
			if (texture(shadowMap_t, v).x < find) {
				v -= vm;
			}
			else {
				v += vm;
			}
			vm /= 2;
		}
//		sshadow = 100*length(projCoords.xy - v);
	}





    float diffuse = max(dot(lightDir, normal), 0.0);

	float ambient = 0.5;

	float spec = 0.0;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float lighting = (ambient + (1.0 - vis) * (diffuse + spec));
//	color = texture(texture_t, fs_in.TexCoords) * vec4(lighting, lighting, lighting, 1.0);

	// float g = p/3000;
	float g = n/  3000;
	float r = m / 3000;
	float b = texture(shadowMap_t, projCoords.xy).x < projCoords.z - 0.00002 ? 1 : 0;
	color = vec4(r, g, b, 1.0);
	color *= sshadow;
	color.w = 1.0;
}
