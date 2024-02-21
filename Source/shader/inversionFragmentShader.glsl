#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uFrameBuffer;

void main() {
	FragColor = vec4(vec3(1.0 - texture(uFrameBuffer, vTexCoord)), 1.0);
}