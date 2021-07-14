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

vec2 poissonDisk[30] = vec2[](
	vec2(0.79562, 0.28269),
	vec2(0.93672, 0.04397),
	vec2(-0.10852, 0.63651),
	vec2(-0.90926, 0.22873),
	vec2(0.62748, 0.36185),
	vec2(0.43553, -0.28124),
	vec2(-0.44851, -0.13695),
	vec2(-0.73506, -0.16161),
	vec2(0.21257, -0.06946),
	vec2(0.95367, 0.30084),
	vec2(0.56038, -0.23479),
	vec2(0.70324, 0.47806),
	vec2(0.18177, -0.09481),
	vec2(0.36958, 0.13068),
	vec2(0.28371, -0.27279),
	vec2(0.05546, -0.73796),
	vec2(-0.19016, 0.94890),
	vec2(-0.81449, 0.14754),
	vec2(-0.49915, -0.13888),
	vec2(-0.48063, 0.73515),
	vec2(0.07044, -0.35392),
	vec2(0.51671, 0.40884),
	vec2(0.08652, -0.45944),
	vec2(0.62789, -0.77423),
	vec2(0.01471, -0.94871),
	vec2(-0.60392, -0.34523),
	vec2(0.57929, 0.81065),
	vec2(-0.16206, -0.08698),
	vec2(0.05689, -0.42763),
	vec2(-0.16079, 0.14143)
);

vec2 circle[16] = vec2[](
	vec2(0.38268, 0.92388),
	vec2(0.70711, 0.70711),
	vec2(0.92388, 0.38268),
	vec2(1.00000, 0.00000),
	vec2(0.92388, -0.38268),
	vec2(0.70711, -0.70711),
	vec2(0.38268, -0.92388),
	vec2(0.00000, -1.00000),
	vec2(-0.38268, -0.92388),
	vec2(-0.70711, -0.70711),
	vec2(-0.92388, -0.38268),
	vec2(-1.00000, -0.00000),
	vec2(-0.92388, 0.38268),
	vec2(-0.70711, 0.70711),
	vec2(-0.38268, 0.92388),
	vec2(0.00000, 1.00000)
);


void main(){
	float vis = 0.0;
	float bias = 0.00005;

	vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

	vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//	bias /= 9.0;

//	if (texture(shadowMap_t, projCoords.xy).x < (projCoords.z - bias)){
//		vis = 1.0;
//	}
//
//	float prox = 3.0;
//
//	float p = 1 / (1 - texture(shadowMap_t, projCoords.xy).x);
//	vec2 dir;
//	float m2 = p;
//	for (int i = 0; i < 8; i++){
//		float f = 1 / (1 - texture(shadowMap_t, projCoords.xy + circle[i] / prox).x);
//
////		float d = abs(p - f) / f;
//
//		if (f < m2) {
//			m2 = f;
//			dir = circle[i] / prox;
//			vis = 1.0;
//		}
//	}

//	int c = 0;
////	if (m2 != 100000.0){
//	if (m2 != p){
//		float target = (texture(shadowMap_t, projCoords.xy + dir).x + projCoords.z) / 2.0;
//		int N = 3;
//		vec2 p = projCoords.xy + dir/2;
//		dir /= 4.0;
//		for (int i = 0; i < N; i++){
//			if (texture(shadowMap_t, p).x < target - 0.001) {
//				p += dir;
//				c++;
//			}
//			else {
//				p -= dir;
//			}
//			dir /= 2.0;
//		}
//
////		vis = length(dir) / 40.0;
////		vis = length(projCoords.xy - p)*80;
////		vis = c / N;
//		vis = 1.0;
//	}

//	float p = 1 / (1 - texture(shadowMap_t, projCoords.xy).x);
//
//	float prox = 9.0;
//	int total = 30;
//	for (int i = 0; i < total; i++){
//		float f = 1 / (1 - texture(shadowMap_t, projCoords.xy + poissonDisk[i] / prox).x);
//
//		float d = (p - f) / f;
//
//		if (texture(shadowMap_t, projCoords.xy + poissonDisk[i] / prox).x < projCoords.z - bias) {
//			prox /= d;
//			vis += (1.0 / total);
//		}
//	}
	float p = projCoords.z;
	float prox = 70.0;
	float begin_prox = prox;

	for (int i = 0; i < 16; i++) {
		float f = texture(shadowMap_t, projCoords.xy + circle[i] / begin_prox).x;

		if (f < p - bias) {
			float p2 = (1 / (1 - p));
			float f2 = (1 / (1 - f));
			float d = (p2 - f2)/ f2;
			prox = max(prox, begin_prox/ d);
		}
	}

	vis = 0;
	for (int i=0;i<30;i++) {
	  if ( texture( shadowMap_t, projCoords.xy + poissonDisk[i]/prox).x  <  projCoords.z-bias ){
		vis +=1.0/30;
	  }
	}

    float diffuse = max(dot(lightDir, normal), 0.0);

	float ambient = 0.5;

	float spec = 0.0;

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float lighting = (ambient + (1.0 - vis) * (diffuse + spec));
	color = texture(texture_t, fs_in.TexCoords) * vec4(lighting, lighting, lighting, 1.0);
}
