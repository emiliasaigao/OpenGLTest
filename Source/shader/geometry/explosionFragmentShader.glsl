#version 330 core

in vec2 oTexCoord;
out vec4 FragColor;

struct Meterial {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_ambient1;
};

uniform Meterial meterial;


void main() {

	vec3 color = 0.5 * (texture(meterial.texture_diffuse1, oTexCoord).rgb +
		texture(meterial.texture_specular1, oTexCoord).rgb +
		texture(meterial.texture_ambient1, oTexCoord).rgb);
	FragColor = vec4(color, 1.0);
}