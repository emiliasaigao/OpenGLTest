#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 vTexCoord;
} gs_in[];

out vec2 oTexCoord;

uniform float time;

vec3 getNormal() {
	vec3 a = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
	vec3 b = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
	return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 1.0;
	vec3 direction = normal * ((sin(time * 2) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}



void main() {
    vec3 normal = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    oTexCoord = gs_in[0].vTexCoord;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    oTexCoord = gs_in[1].vTexCoord;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    oTexCoord = gs_in[2].vTexCoord;
    EmitVertex();
    EndPrimitive();
}