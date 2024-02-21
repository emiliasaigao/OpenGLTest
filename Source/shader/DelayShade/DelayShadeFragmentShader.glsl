#version 330 core

out vec4 FragColor;

in vec2 vTexCoord;

// 均基于观察空间
uniform sampler2D uAlbedo;
uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uSSAOblurTexture;
uniform bool useSSAO;

struct Light {
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
	float Radius;
};

uniform Light light;

void main() {

	// FragColor = vec4(texture(uAlbedo, vTexCoord).rgb, 1.0);

	vec3 fragPos = texture(uPosition, vTexCoord).rgb;
	vec3 normal = texture(uNormal, vTexCoord).rgb;
	vec3 diff = texture(uAlbedo, vTexCoord).rgb;
	vec3 L = light.Position - fragPos;
	vec3 N = normalize(normal);
	// 观察空间中摄像机在(0,0,0)处
	vec3 V = vec3(0.) - fragPos;
	vec3 H = normalize(L + V);
	vec3 ambient;
	if (useSSAO) {
		ambient = vec3(texture(uSSAOblurTexture, vTexCoord).r);
	}
	else {
		ambient = vec3(1.);
	}
	/*vec3 diffuse = max(dot(L, N), 0.0) * diff * light.Color;
	vec3 specular = pow(max(dot(H, N), 0.0), 32) * light.Color;

	float dist = length(light.Position - fragPos);
	float attenuation = 1.0 / (1.0 + light.Linear * dist + light.Quadratic * dist * dist);
	diffuse *= attenuation;
	specular *= attenuation;
	vec3 lighting = ambient;
	lighting *= diffuse + specular;*/
	vec3 lighting = ambient;
	lighting *= diff;
	FragColor = vec4(lighting, 1.0);
}