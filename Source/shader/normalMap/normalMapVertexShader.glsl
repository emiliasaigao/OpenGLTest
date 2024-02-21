#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormModel;

uniform vec3 uLightPos;
uniform vec3 uCameraPos;

out VS_OUT {
	vec3 vFragPos;
	vec2 vTexCoord;
	vec3 vTBNLightPos;
	vec3 vTBNCameraPos;	
	vec3 vTBNFragPos;
} vs_out;

void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPos,1.0);
	vec3 T = normalize(uNormModel * aTangent);
	vec3 B = normalize(uNormModel * aBitangent);
	vec3 N = normalize(uNormModel * aNormal);
	mat3 TBN = mat3(T, B, N);
	TBN = transpose(TBN);
	vs_out.vFragPos = aPos;
	vs_out.vTexCoord = aTexCoord;
	vs_out.vTBNLightPos = TBN * uLightPos;
	vs_out.vTBNCameraPos = TBN * uCameraPos;
	vs_out.vTBNFragPos = TBN * vec3(uModel * vec4(aPos,1.0));
}
