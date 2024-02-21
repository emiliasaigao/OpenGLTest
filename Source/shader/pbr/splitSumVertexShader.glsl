#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vFragPos;
out vec3 vNormal;


uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
	vTexCoord = aTexCoord;
	vFragPos = vec3(uModel * vec4(aPos, 1.0));
	mat3 normModel = transpose(inverse(mat3(uModel)));
	vNormal = normModel * aNormal;
}