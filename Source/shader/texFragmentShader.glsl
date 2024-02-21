//#version 330 core
//// in vec3 myColor;
//in vec2 myTexCoord;
//out vec4 FragColor;
//
//uniform sampler2D uTexture1;
//uniform sampler2D uTexture2;
//uniform float mixvalue;
//
//
//void main() {
//	FragColor = mix(texture(uTexture1,myTexCoord),texture(uTexture2,myTexCoord), mixvalue);
//}
#version 330 core
out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D texture1;

void main()
{
    //FragColor = vec4(textureLod(texture1, vTexCoord, 5.0).rgb ,1.0);
    FragColor = vec4(texture(texture1, vTexCoord).rgb ,0.0);
    //FragColor = vec4(vec3(texture(texture1, vTexCoord).r) ,1.0);
    //FragColor = vec4(vec3(texture(texture1, vec3(vTexCoord, 1.0)).rbg) ,1.0);
}