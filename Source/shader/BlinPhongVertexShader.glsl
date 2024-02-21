#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormModel;

out vec3 vNormal;
out vec3 vPosition;
out vec2 vTexCoord;

void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPos,1.f);
	vNormal = uNormModel * aNormal;
	vPosition = vec3(uModel * vec4(aPos,1.0f));
	vTexCoord = aTexCoord;
}