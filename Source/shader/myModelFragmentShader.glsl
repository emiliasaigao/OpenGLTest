#version 330 core

in vec3 vNormal;
in vec3 vPosition;
in vec2 vTexCoord;
in vec4 vFragPos;

out vec4 FragColor;

uniform vec3 uCameraPos;

struct Meterial {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_ambient1;
};

uniform Meterial meterial;
//uniform samplerCube uCubeMap;
//uniform sampler2D uSSAOblurTexture;

void main() {
	// 法向量
	vec3 N = normalize(vNormal);
	// 观察出射方向
	vec3 V = normalize(vPosition - uCameraPos);
	//vec3 R = reflect(V, N);
	//float ratio = 1.00 / 1.52;
	//vec3 R = refract(V, N, ratio);
	//vec3 color = texture(uCubeMap, R).rgb;
	vec3 color = 0.2 * (texture(meterial.texture_diffuse1, vTexCoord).rgb +
		texture(meterial.texture_specular1, vTexCoord).rgb +
		texture(meterial.texture_ambient1, vTexCoord).rgb); 
		
		// 附加SSAO效果	
		/*vec2 ssaoTex = vFragPos.xy / vFragPos.w;
		vec3 color = 0.5 * vec3(texture(uSSAOblurTexture, ssaoTex).r)  + 0.4 * (
		texture(meterial.texture_specular1, vTexCoord).rgb +
		texture(meterial.texture_ambient1, vTexCoord).rgb);*/
	FragColor = vec4(color, 1.0);
}