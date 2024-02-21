#version 330 core
out vec4 FragColor;

uniform vec3 uLightColor;
uniform vec3 uModelColor;

void main() {
	FragColor = vec4((uLightColor * uModelColor),1.f);
}