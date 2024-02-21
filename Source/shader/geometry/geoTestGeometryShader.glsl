#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT{
	vec3 vNormal;
} gs_in[];

const float NORMAL_LINE_LENGTH = 0.02;

void GenerateLine(int index) {
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].vNormal,0.0) * NORMAL_LINE_LENGTH;
	EmitVertex();
	EndPrimitive();
}

void main() {
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}