#version 330 core

in vec2 vTexCoord;

out vec3 FragColor;
const float PI = 3.14159265359;
// �Ͳ�������
float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

// ����
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

// NDF��
float DistributionGGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float nom = a2;
    float dnom = NdotH * NdotH * (a2 - 1.) + 1.;
    dnom = PI * dnom * dnom;
    return nom / dnom;
}

// G��
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness;
    float k = (r * r) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(float roughness, float NoV, float NoL) {
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);

    return ggx1 * ggx2;
}

// F��
vec3 fresnelSchlick(float VdotH, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

vec3 IntegrayBRDF(float NdotV, float roughness) {
    vec3 V; 
    V.x = sqrt(1 - NdotV * NdotV);
    V.y = 0.;
    V.z = NdotV;
    float A = 0.;
    float B = 0.;
    float C = 0.;
    vec3 N = vec3(0.0, 0.0, 1.0);
    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        // ��ʵ�൱������ڷ���H��V�ķ�������
        vec3 L = normalize(2.0 * dot(V, H) * H - V);
        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.) {
            float G = GeometrySmith(roughness, NdotV, NdotL);
            float D = DistributionGGX(NdotH, roughness);
            /*
                ע�⿴G_Vis����ô���
                ����brdf = (D * G) / (4 * NdotL * NdotV);
                ���滹ʣ��NdotL
                Ȼ�����ؿ�������ǡ���(����ֵ/��������)��
                ���ڲ�����Χ�Ʒ��߽��еģ�������ܶ�Ҳ��D���йأ���֮pdf�����
                pdf = D * NdotH / 4 * VdotH;
                G_Vis = brdf * NdotL / pdf
                      = (D * G) / (4 * NdotL * NdotV) * NdotL * 4 * VdotH / (D * NdotH)
                      = (G * VdotH) / (NdotH * NdotV)
            */

            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1 - VdotH, 5.0);
            A += (1 - Fc) * G_Vis;
            B += Fc * G_Vis;
            C += G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    C /= float(SAMPLE_COUNT);
    return vec3(A, B, C);
}

void main() {
    FragColor = IntegrayBRDF(vTexCoord.x, vTexCoord.y);
}