#version 330 core

layout(location = 0) out vec3 oPositionDepth;
layout(location = 1) out vec3 oNormal;
layout(location = 2) out vec3 oAlbedo;

in VS_OUT{
	vec3 vFragPos;
	vec3 vNormal;
	vec2 vTexCoord;
}fs_in;

struct Meterial {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

uniform Meterial meterial;

void main() {
	oPositionDepth = fs_in.vFragPos;
	oNormal = normalize(fs_in.vNormal);
	oAlbedo = texture(meterial.texture_diffuse1, fs_in.vTexCoord).rgb;
}