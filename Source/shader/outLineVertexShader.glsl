#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uOutLine;

void main() {
	vec3 pos = aPos + uOutLine * normalize(aNormal);
	gl_Position = uProjection * uView * uModel * vec4(pos, 1.f);
}