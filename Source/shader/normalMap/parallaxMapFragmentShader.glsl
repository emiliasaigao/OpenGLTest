#version 330 core

uniform sampler2D uNormalMap;
uniform	sampler2D uTexture;
uniform sampler2D uDispMap;
uniform float uHeightScale;
uniform bool parallax;

in VS_OUT{
	vec3 vFragPos;
	vec2 vTexCoord;
	vec3 vTBNLightPos;
	vec3 vTBNCameraPos;
	vec3 vTBNFragPos;
} fs_in;

out vec4 FragColor;

vec3 calcPointLight(vec3 N, vec3 V, vec2 texCoord) {
	vec3 L = normalize(fs_in.vTBNLightPos - fs_in.vTBNFragPos);
	// 半程向量
	vec3 H = normalize(V + L);
	vec2 vTexCoord = texCoord;
	vec3 ambient = 0.2 * texture(uTexture, vTexCoord).rgb;
	vec3 diffuse = max(0.0, dot(N, L)) * texture(uTexture, vTexCoord).rgb;
	vec3 specular = pow(max(0.0, dot(N, H)), 32)
		* vec3(0.2f);

	vec3 result = ambient + diffuse + specular;
	return result;
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) {
	//float height = texture(uDispMap, texCoord).r;
	//vec2 p = viewDir.xy / viewDir.z * (height * uHeightScale);
	//return texCoord - p;

	// 二分法寻找视线与深度贴图的交点
	const int numLayers = 100;
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;

	vec2 P = viewDir.xy * uHeightScale;
	vec2 deltaTexCoords = P / numLayers;


	int left = 0;
	int right = numLayers;
	while (left < right) {
		int mid = left + (right - left) / 2;
		vec2 currentTexCoords = texCoords - mid * deltaTexCoords;
		float currentDepthMapValue = texture(uDispMap, currentTexCoords).r;
		float currentLayerDepth = mid * layerDepth;
		if (abs(currentLayerDepth - currentDepthMapValue) < layerDepth)
			return currentTexCoords;
		else if (currentLayerDepth > currentDepthMapValue)
			right = mid - 1;
		else
			left = mid + 1;
	}
	return texCoords - left * deltaTexCoords;

}

void main() {
	// 观察出射方向
	vec3 V = normalize(fs_in.vTBNCameraPos - fs_in.vTBNFragPos);
	vec2 newTexCoord = fs_in.vTexCoord;
	if (parallax)
		newTexCoord = parallaxMapping(newTexCoord, V);

	if (newTexCoord.x > 1.0 || newTexCoord.y > 1.0 || newTexCoord.x < 0.0 || newTexCoord.y < 0.0)
		discard;
	// 法向量
	vec3 normal = texture(uNormalMap, newTexCoord).rgb;
	vec3 N = normalize(normal * 2.0 - 1.0);

	vec3 result = calcPointLight(N, V, newTexCoord);

	FragColor = vec4(result, 1.0);
}
//	vec3 viewDir = normalize(fs_in.vTBNCameraPos - fs_in.vTBNFragPos);
//	vec2 texCoords = fs_in.vTexCoord;
//	if (parallax)
//		texCoords = parallaxMapping(texCoords, viewDir);
//
//	// Obtain normal from normal map
//	vec3 normal = texture(uNormalMap, texCoords).rgb;
//	normal = normalize(normal * 2.0 - 1.0);
//
//	// Get diffuse color
//	vec3 color = texture(uTexture, texCoords).rgb;
//	// Ambient
//	vec3 ambient = 0.1 * color;
//	// Diffuse
//	vec3 lightDir = normalize(fs_in.vTBNLightPos - fs_in.vTBNFragPos);
//	float diff = max(dot(lightDir, normal), 0.0);
//	vec3 diffuse = diff * color;
//	// Specular    
//	vec3 halfwayDir = normalize(lightDir + viewDir);
//	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//
//	vec3 specular = vec3(0.2) * spec;
//	FragColor = vec4(ambient + diffuse + specular, 1.0f);
