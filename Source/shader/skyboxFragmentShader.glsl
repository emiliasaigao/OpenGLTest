#version 330 core

in vec3 vTexCoord;
out vec4 FragColor;

uniform samplerCube skybox;

void main() {
    vec3 envColor = textureLod(skybox, vTexCoord, 0.0).rgb;

    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0 / 2.2));
    envColor = clamp(envColor, vec3(0.0), vec3(1.0));
    FragColor = vec4(envColor, 1.0);
}