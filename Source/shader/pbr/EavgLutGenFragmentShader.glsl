#version 330 core

in vec2 vTexCoord;
out float FragColor;

uniform sampler2D uEiMap;
const float SAMPLE_COUNT = 512.;
const float FLAG_SIZE = 1. / SAMPLE_COUNT;

void main() {
	float E_avg = 0.;
	for (int i = 0; i < 512; ++i) {
		E_avg += texture(uEiMap, vec2(i / SAMPLE_COUNT + FLAG_SIZE / 2.0, vTexCoord.y)).b;
	}
	E_avg /= SAMPLE_COUNT;
	FragColor = E_avg;
	//FragColor = texture(uEiMap, vTexCoord).r;
	//FragColor = 1.0;
}