#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uDepthMap;

void main() {
	vec3 DepthValue = vec3(texture(uDepthMap, vTexCoord).r);
	FragColor = vec4(DepthValue, 1.0);
	//FragColor = texture(uDepthMap, vTexCoord);
}