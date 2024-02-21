#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
//layout(location = 3) in mat4 aModelMatrix;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormModel;
uniform mat4 uLightMVP;

out vec3 vPosition;
out vec2 vTexCoord;
out vec4 vPositionFromLight;
out vec3 vNormal;

void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.f);
	vPositionFromLight = uLightMVP * vec4(aPos, 1.0f);
	vPosition = vec3(uModel * vec4(aPos, 1.0f));
	vNormal = uNormModel * aNormal;
	vTexCoord = aTexCoord;
}