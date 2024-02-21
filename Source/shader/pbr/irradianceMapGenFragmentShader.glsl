#version 330 core

in vec3 vLocalPos;
out vec4 FragColor;

uniform samplerCube uEnvCubeMap;
const float PI = 3.14159265359;

void main() {
    vec3 normal = normalize(vLocalPos);
	vec3 irradiance;
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // 在切线空间中均匀离散采样
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // 转换为世界空间方向
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            // *cos(theta)*sin(theta)其实是乘以采样面积
            irradiance += texture(uEnvCubeMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    FragColor = vec4(irradiance, 1.0); 
    //FragColor = vec4(texture(uEnvCubeMap, vLocalPos).rgb, 1.0);
    //normal = (normal + vec3(1.0)) / 2;
    //FragColor = vec4(vec3(normal.r),1.0);
}