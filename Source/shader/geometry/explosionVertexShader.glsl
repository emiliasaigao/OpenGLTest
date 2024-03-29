#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormModel;


out VS_OUT{
	vec2 vTexCoord;
} vs_out;

void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.f);
	vs_out.vTexCoord = aTexCoord;
}