#version 330 core

in vec2 vTexCoord;
in vec3 vFragPos;
in vec3 vNormal;
out vec4 FragColor;

uniform vec3 uCameraPos;
struct Light {
	vec3 Position;
	vec3 Radiance;
};

uniform Light light[4];
//uniform bool usePointLight;

uniform vec3 uAlbedo;
uniform float uMetallic;
uniform float uRoughness;
uniform samplerCube uIrrandianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D LUT;

const float MAX_REFLECTION_LOD = 5.0;
const float pi = 3.14159265359;

// NDFÏî
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = dot(N, H);
	float nom = a2;
	float dnom = NdotH * NdotH * (a2 - 1.) + 1.;
	dnom = pi * dnom * dnom;
	return nom / dnom;
}

// GÏî
float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

float GeometrySmith(float roughness, float NoV, float NoL) {
	float ggx2 = GeometrySchlickGGX(NoV, roughness);
	float ggx1 = GeometrySchlickGGX(NoL, roughness);

	return ggx1 * ggx2;
}

// FÏî
vec3 fresnelSchlick(float VdotH, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
	vec3 N = normalize(vNormal);
	vec3 V = normalize(uCameraPos - vFragPos);
	vec3 L = normalize(reflect(-V, N));
	float NdotV = max(dot(N, V), 0.0);
	float roughness = uRoughness;
	float metallic = uMetallic;
	vec3 albedo = uAlbedo;
	albedo = pow(albedo, vec3(2.2));

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 L_prefil = textureLod(uPrefilterMap, L, roughness * MAX_REFLECTION_LOD).rgb;
	vec3 L_trans = F0 * texture(LUT, vec2(NdotV, roughness)).r
		+ vec3(texture(LUT, vec2(NdotV, roughness)).b);

	vec3 Lo = L_prefil * L_trans;

	for (int i = 0; i < 4; ++i) {
		vec3 L = normalize(light[i].Position - vFragPos);
		vec3 H = normalize(L + V);
		float NdotL = max(dot(N, L), 0.);
		float VdotH = max(dot(V, H), 0.);
		float distance = length(light[i].Position - vFragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = light[i].Radiance * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(roughness, NdotV, NdotL);
		vec3 F = fresnelSchlick(VdotH, F0);
		vec3 nom = NDF * G * F;
		float denom = 4 * max(NdotV, 0.) * max(NdotL, 0.) + 0.0001;
		vec3 specular = nom / denom;

		vec3 Ks = F;
		vec3 Kd = vec3(1.0) - Ks;
		Kd *= 1.0 - metallic;
		Lo += (Kd * albedo / pi + Ks * specular) * radiance * NdotL;
	}
	vec3 Ks = fresnelSchlick(max(dot(N, V), 0.0), F0);
	vec3 Kd = 1.0 - Ks;
	vec3 irradiance = texture(uIrrandianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 ambient = Kd * diffuse;
	vec3 specular = Lo;

	vec3 color = diffuse + specular;
	//vec3 ambient = L_prefil * L_trans * albedo;
	//vec3 color = ambient;
	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));
	FragColor = vec4(color, 1.0);
}