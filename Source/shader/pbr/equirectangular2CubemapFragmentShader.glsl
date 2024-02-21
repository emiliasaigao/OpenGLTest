#version 330 core
out vec4 FragColor;
in vec3 vLocalPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    // 根据片元局部坐标来计算球面角theta和phi，进而在等距柱状投影图上采样
    vec2 uv = SampleSphericalMap(normalize(vLocalPos));
    vec3 color = texture(equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}