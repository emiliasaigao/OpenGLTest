#version 330 core

in vec3 vWorldPos; // 片元在世界坐标系中的坐标
out vec4 FragColor;
const float pi = 3.14159265359;
uniform sampler2D equirectangularMap;

void main() {
    vec3 worldPos = normalize(vWorldPos);
    // 计算相机坐标的球坐标
    float theta = atan(worldPos.z, worldPos.x);
    float phi = acos(worldPos.y / length(worldPos));

    // 将球坐标映射到纹理坐标
    float u = (theta + pi) / (2.0 * pi);
    float v = phi / pi;

    // 输出纹理坐标
    vec2 texCoord = vec2(u, v);

    FragColor = textureLod(equirectangularMap, texCoord, 0.);
}