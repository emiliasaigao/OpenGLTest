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

uniform sampler2D uAlbedoMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uNormalMap;

uniform samplerCube uIrrandianceMap;
uniform samplerCube uPrefilterMap;
uniform sampler2D LUT;
uniform sampler2D Eavg;

const float MAX_REFLECTION_LOD = 5.0;
const float PI = 3.14159265359;

// NDF项
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = dot(N, H);
	float nom = a2;
	float dnom = NdotH * NdotH * (a2 - 1.) + 1.;
	dnom = PI * dnom * dnom;
	return nom / dnom;
}

// G项
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

// F项
vec3 fresnelSchlick(float VdotH, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 getNormalFromMap() {
	vec3 tangentNormal = texture(uNormalMap, vTexCoord).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(vFragPos);
	vec3 Q2 = dFdy(vFragPos);
	vec2 st1 = dFdx(vTexCoord);
	vec2 st2 = dFdy(vTexCoord);

	vec3 N = normalize(vNormal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

// 近似计算Favg的算法
//https://blog.selfshadow.com/publications/s2017-shading-course/imageworks/s2017_pbs_imageworks_slides_v2.pdf
vec3 AverageFresnel(vec3 r, vec3 g)
{
	return vec3(0.087237) + 0.0230685 * g - 0.0864902 * g * g + 0.0774594 * g * g * g
		+ 0.782654 * r - 0.136432 * r * r + 0.278708 * r * r * r
		+ 0.19744 * g * r + 0.0360605 * g * g * r - 0.2586 * g * r * r;
}

void main() {
	vec3 N = getNormalFromMap();
	vec3 V = normalize(uCameraPos - vFragPos);
	vec3 L = normalize(reflect(-V, N));
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float roughness = texture(uRoughnessMap, vTexCoord).r;
	float metallic = texture(uMetallicMap, vTexCoord).r;
	vec3 albedo = texture(uAlbedoMap, vTexCoord).rgb;
	albedo = pow(albedo, vec3(2.2));

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	// 计算附加brdf项
	/*************************************************************/
	// 查询E(μ)的贴图获取E(μo)和E(μi)
	vec3 E_o = vec3(texture2D(LUT, vec2(NdotL, roughness)).b);
	vec3 E_i = vec3(texture2D(LUT, vec2(NdotV, roughness)).b);

	vec3 E_avg = vec3(texture2D(Eavg, vec2(0, roughness)).r);
	// copper
	vec3 edgetint = vec3(0.827, 0.792, 0.678);
	vec3 F_avg = AverageFresnel(albedo, edgetint);

	// 补全多次弹射损失的能量
	vec3 f_ms = (vec3(1.0) - E_o) * (vec3(1.0) - E_i) / (PI * (vec3(1.0) - E_avg));
	// 考虑F项（颜色）影响的能量损失（应该损失的）
	vec3 f_add = F_avg * E_avg / (vec3(1.0) - F_avg * (vec3(1.0) - E_avg));
	vec3 f_kulla = f_ms * f_add;
	vec3 X = (vec3(1.0) - E_o) / (PI * (vec3(1.0) - E_avg)) * f_add;
	/*************************************************************/

	vec3 L_prefil = textureLod(uPrefilterMap, L, roughness * MAX_REFLECTION_LOD).rgb;
	vec3 L_trans = F0 * texture(LUT, vec2(NdotV, roughness)).r
		+ vec3(texture(LUT, vec2(NdotV, roughness)).g);

	vec3 L_trans_kulla = PI * (1 - E_avg) * X;
	
	vec3 Lo = L_prefil * (L_trans + L_trans_kulla);

	/*for (int i = 0; i < 4; ++i) {
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
		Lo += (Kd * albedo / PI + Ks * specular) * radiance * NdotL;
	}*/
	vec3 Ks = fresnelSchlick(max(dot(N, V), 0.0), F0);
	vec3 Kd = 1.0 - Ks;
	Kd *= 1 - metallic;
	vec3 irradiance = texture(uIrrandianceMap, N).rgb;
	vec3 diffuse = irradiance * albedo;
	vec3 ambient = Kd * diffuse;
	vec3 specular = Lo;

	vec3 color = diffuse + specular;
	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));
	FragColor = vec4(color, 1.0);
}