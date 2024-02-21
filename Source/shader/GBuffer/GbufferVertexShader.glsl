#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out VS_OUT {
	// 均为观察空间的变量
	vec3 vFragPos;
	vec3 vNormal;
	vec2 vTexCoord;
}vs_out;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uView;

void main() {
	vec4 viewPos = uView * uModel * vec4(aPos, 1.0);
	vs_out.vFragPos = viewPos.xyz;
	gl_Position = uProjection * viewPos;
	mat3 normMatrix = transpose(inverse(mat3(uView * uModel)));
	vs_out.vNormal = normMatrix * aNormal;
	vs_out.vTexCoord = aTexCoord;
	//gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
	//vs_out.vFragPos = vec3(uModel * vec4(aPos, 1.0));
	//mat3 normMatrix = transpose(inverse(mat3(uModel)));
	//vs_out.vNormal = normMatrix * aNormal;
}