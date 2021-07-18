#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out float shadow;

uniform sampler2D texture_t;
uniform sampler2D shadowMap_t;

uniform vec3 viewPos;
uniform float lightFar;
uniform vec3 lightPos;

in VS_OUT {
    vec3 fragWorldPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragLightPos;
} fs_in;

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

	vec3 projCoords = fs_in.fragLightPos.xyz / fs_in.fragLightPos.w;
    projCoords = projCoords * 0.5 + 0.5;

	vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(lightPos - fs_in.fragWorldPos);
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	float p = 1 / (1 - projCoords.z );
	float pp = 1 / (1 - texture(shadowMap_t, projCoords.xy).x);

	float prox = 100.0;

	float m = p;
	vec2 vm = vec2(0);
	int vmcount = 0;
	float n = pp;
	vec2 vn = vec2(0);
	int vncount = 0;
	for (int i = 0; i < 17; i++) {
		float f = 1 / (1 - texture(shadowMap_t, projCoords.xy + circle[i] / prox).x);

		if (p - f > 1) {
			m = f;
			vm += circle[i] / prox;
			vmcount++;
		}

		if (p - f < 1) {
			n = f;
			vn += circle[i] / prox;
			vncount++;
		}
	}

	vm /= vmcount;
	vm = normalize(vm) / prox;

	vn /= vncount;
	vn = normalize(vn) / prox;

	float sshadow = 1.0;

	if (m != p) {
		vec2 v = projCoords.xy + vm / 2;
		float find = (projCoords.z + texture(shadowMap_t, projCoords.xy + vm).x)/2;
		vm /= 4;
		for (int i = 0; i < 4; i++){
			v += sign(texture(shadowMap_t, v).x - find) * vm;
			vm /= 2;
		}

		for (int i = 0; i < 3; i++) {
			if (texture(shadowMap_t, v).x > find) {
				v += vm;
				vm /= 2;
			}
		}

		float edgeDistance = 1 / (1 - texture(shadowMap_t, v).x);
		float currentDistance = 1 / (1 - texture(shadowMap_t, projCoords.xy).x);
		float theScale = (currentDistance - edgeDistance) / edgeDistance;
//		theScale = clamp(theScale, 0.0, 1.0);
//		theScale /= 2.0;

		float theLength = prox * length(projCoords.xy - v);

		if (theLength < theScale){
			sshadow = 1 - (1 - prox * length(projCoords.xy - v) / theScale) / 2;
//			sshadow = 1 - (1 - prox * length(projCoords.xy - v) / theScale) / 2;
		}
	}


	if (n != p) {
		vec2 v = projCoords.xy + vn/2;
		float find = (texture(shadowMap_t, projCoords.xy).x + texture(shadowMap_t, projCoords.xy + vn).x)/2;
		vn /= 4;
		for (int i = 0; i < 4; i++){
			v -= sign(texture(shadowMap_t, v).x - find) * vn;
//			if (texture(shadowMap_t, v).x > find) {
//				v -= vn;
//			}
//			else {
//				v += vn;
//			}
			vn /= 2;
		}

		for (int i = 0; i < 3; i++) {
			if (texture(shadowMap_t, v).x < find) {
				v += vn;
				vn /= 2;
			}
		}

		float edgeDistance = 1 / (1 - texture(shadowMap_t, v).x);
		float currentDistance = 1 / (1 - texture(shadowMap_t, projCoords.xy).x);
		float theScale = -(currentDistance - edgeDistance) / edgeDistance;
//		theScale = clamp(theScale, 0.0, 1.0);
//		theScale /= 2.0;

		float theLength = prox * length(projCoords.xy - v);

		if (theLength < theScale){

			sshadow =  0.5 - ( prox * length(projCoords.xy - v) / theScale) / 2;
		}
//			sshadow = prox * length(projCoords.xy - v);

//		sshadow = prox * length(projCoords.xy - v) * (theScale);
//		sshadow = prox * length(projCoords.xy - v) * (theScale);
//		sshadow = clamp(sshadow, 0, 1);
//		sshadow = theScale;
	}


	if (sshadow == 1.0){
		if (texture(shadowMap_t, projCoords.xy).x < projCoords.z - 0.000002){
			sshadow = 0.0;
		}
	}


    float diffuse = max(dot(lightDir, normal), 0.0);

	float ambient = 0.1;

	float spec = 0.0;

	vec3 viewDir = normalize(viewPos - fs_in.fragWorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	float spotPower = clamp(1 - 2 * length(projCoords.xy - vec2(0.5)), 0, 1) * 2.2;
	float distance_to_light = 1 - (fs_in.fragLightPos.z / lightFar);
	float distance_to_light2 = length(fs_in.fragWorldPos.xyz - lightPos) / lightFar;
	distance_to_light2 *= distance_to_light2;
	distance_to_light2 = 1 - distance_to_light2;
    float lighting = (ambient + (sshadow) * (diffuse + spec * sshadow) * spotPower * distance_to_light2);
//	lighting = 1 - distance_to_light2;
//	if (lighting > 1){
//	lighting = 0;}
//
//	float lighting = sshadow;
//	color = texture(texture_t, fs_in.texCoords) * vec4(lighting, lighting, lighting, 1.0);

//	// float g = p/3000;
	float g = n / 3000;
	float r = m / 3000;
//	float b = texture(shadowMap_t, projCoords.xy).x < projCoords.z - 0.00002 ? 1 : 0;
//	color = vec4(r, g, b, 1.0);
//	color *= sshadow;
//	color.w = 1.0;

//	gl_FragDepth = fs_in.fragWorldPos.z;

	color = texture(texture_t, fs_in.texCoords);
//	color = texture(texture_t, fs_in.texCoords);
//	color = vec4(lighting);
	color.w = 1.0;
	shadow = clamp(lighting, 0, 3);
	shadow *= 2;
//	color.g += r/3;
//	color.b += g/3;
}
