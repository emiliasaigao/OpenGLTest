#version 330 core
layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec3 aColor;
//layout(location = 2) in vec2 aTexCoord;
layout(location = 1) in vec2 aTexCoord;
out vec3 myColor;
out vec2 myTexCoord;
// uniform mat4 transform;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;


void main() {
	gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
	//myColor = aColor;
	myTexCoord = aTexCoord;
}