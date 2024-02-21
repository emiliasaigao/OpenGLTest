#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uFrameBuffer;

void main() {
    FragColor = texture(uFrameBuffer, vTexCoord);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(average, average, average, 1.0);
}