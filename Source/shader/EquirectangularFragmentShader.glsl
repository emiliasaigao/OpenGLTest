#version 330 core

in vec3 vWorldPos; // ƬԪ����������ϵ�е�����
out vec4 FragColor;
const float pi = 3.14159265359;
uniform sampler2D equirectangularMap;

void main() {
    vec3 worldPos = normalize(vWorldPos);
    // ������������������
    float theta = atan(worldPos.z, worldPos.x);
    float phi = acos(worldPos.y / length(worldPos));

    // ��������ӳ�䵽��������
    float u = (theta + pi) / (2.0 * pi);
    float v = phi / pi;

    // �����������
    vec2 texCoord = vec2(u, v);

    FragColor = textureLod(equirectangularMap, texCoord, 0.);
}