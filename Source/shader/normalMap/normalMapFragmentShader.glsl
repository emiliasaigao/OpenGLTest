#version 330 core

uniform sampler2D uNormalMap;

in VS_OUT{
	vec3 vFragPos;
	vec2 vTexCoord;
	vec3 vTBNLightPos;
	vec3 vTBNCameraPos;
	vec3 vTBNFragPos;
} fs_in;

struct Meterial {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct PointLight {
	vec3 diffuse;
	vec3 specular;
	float intensity;

	float Kc;
	float Kl;
	float Kq;
};

uniform Meterial meterial;
uniform PointLight pLight;

out vec4 FragColor;

vec3 calcPointLight(PointLight light, vec3 N, vec3 V) {
	vec3 L = normalize(fs_in.vTBNLightPos - fs_in.vTBNFragPos);
	// 半程向量
	vec3 H = normalize(V + L);
	vec2 vTexCoord = fs_in.vTexCoord;
	vec3 ambient = 0.1 * texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 diffuse = light.intensity * max(0.0, dot(N, L)) * light.diffuse
		* texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 specular = light.intensity * pow(max(0.0, dot(N, H)), meterial.shininess)
		* light.specular * texture(meterial.texture_diffuse1, vTexCoord).rgb;
	float distance = length(fs_in.vTBNLightPos - fs_in.vTBNFragPos);
	float attenuation = 1.0 /*/ (light.Kc + light.Kl * distance + light.Kq * distance * distance)*/;
	vec3 result = (ambient + diffuse + specular) * attenuation;
	return result;
}

void main() {
	// 法向量
	vec3 normal = texture(uNormalMap,fs_in.vTexCoord).rgb;
	vec3 N = normalize(normal * 2.0 - 1.0);
	// 观察出射方向
	vec3 V = normalize(fs_in.vTBNCameraPos - fs_in.vTBNFragPos);

	vec3 result = calcPointLight(pLight, N, V);

	FragColor = vec4(result, 1.0);
}