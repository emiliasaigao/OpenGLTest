#version 330 core

in vec3 vNormal;
in vec3 vPosition;
in vec2 vTexCoord;

struct Meterial {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct PointLight {
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	float intensity;

	float Kc;
	float Kl;
	float Kq;
};

struct FlashLight {
	vec3 position;
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	float intensity;
	float cutOff;
	float outerCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct DirecLight {
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
	float intensity;
};

//#define POINTLIGHT_COUNT 0
uniform vec3 uCameraPos;
uniform Meterial meterial;
//uniform PointLight pLight[POINTLIGHT_COUNT];
uniform FlashLight fLight;
uniform DirecLight dLight;

out vec4 FragColor;

vec3 calcDirectLight(DirecLight light, vec3 N, vec3 V) {
	vec3 L = normalize(light.direction);
	// 半程向量
	vec3 H = normalize(V + L);
	vec3 ambient = 0.02 * texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 diffuse = light.intensity * max(0.0, dot(N, L)) * light.diffuse
		* texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 specular = light.intensity * pow(max(0.0, dot(N, H)), meterial.shininess)
		* light.specular * texture(meterial.texture_specular1, vTexCoord).rgb;
	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 calcPointLight(PointLight light, vec3 N, vec3 V) {
	vec3 L = normalize(light.position - vPosition);;
	// 半程向量
	vec3 H = normalize(V + L);
	vec3 ambient = 0.02 * texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 diffuse = light.intensity * max(0.0, dot(N, L)) * light.diffuse
		* texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 specular = light.intensity * pow(max(0.0, dot(N, H)), meterial.shininess)
		* light.specular * texture(meterial.texture_specular1, vTexCoord).rgb;
	float distance = length(light.position - vPosition);
	float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * distance * distance);
	vec3 result = (ambient + diffuse + specular) * attenuation;
	return result;
}

vec3 calcFlashLight(FlashLight light, vec3 N, vec3 V) {
	vec3 L = normalize(light.direction);
	// 半程向量
	vec3 H = normalize(V + L);
	vec3 ambient = 0.2 * texture(meterial.texture_diffuse1, vTexCoord).rgb;
	
	float theta = dot(V, normalize(-L));
	float epsilon = light.cutOff - light.outerCutOff;
	float factor = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	vec3 diffuse = light.intensity * factor * max(0.0, dot(N, -L)) 
		* light.diffuse * texture(meterial.texture_diffuse1, vTexCoord).rgb;
	vec3 specular = light.intensity * factor * pow(max(0.0, dot(N, H)), meterial.shininess)
		* light.specular * texture(meterial.texture_specular1, vTexCoord).rgb;
	float distance = length(light.position - vPosition);
	float attenuation = 1.0 / (light.Kc + light.Kl * distance + light.Kq * distance * distance);
	vec3 result = (ambient + diffuse + specular) * attenuation;
	return result;
}


void main() {
	vec3 color = texture(meterial.texture_diffuse1,vTexCoord).rgb;
	// 法向量
	vec3 N = normalize(vNormal);
	// 观察出射方向
	vec3 V = normalize(uCameraPos - vPosition);

	
	// 定向光照
	vec3 result = calcDirectLight(dLight, N, V);
	// 点光源
	//for (int i = 0; i < POINTLIGHT_COUNT; i++)
		//result += calcPointLight(pLight[i], N, V);
	// 聚光
	result += calcFlashLight(fLight, N, V); 

	//FragColor = vec4(result, vec3(1.0 / 2.2)), 1.0);
	FragColor = vec4(result , 1.0);
}