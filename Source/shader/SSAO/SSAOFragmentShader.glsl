#version 330 core

out float FragColor;

in vec2 vTexCoord;

// 位置、深度、法线贴图均是基于观察空间的
uniform sampler2D uPosition;
uniform sampler2D uNormal;
uniform sampler2D uNoiseMap;

uniform vec3 samples[64];
uniform mat4 uProjection;

const float kernelSize = 64.0;
const float radius = 2.0;
const float bias = 0.025;

const vec2 noiseScale = vec2(800.0 / 4.0, 600.0 / 4.0);

void main() {
	vec3 fragPos = texture(uPosition, vTexCoord).rgb;
	vec3 normal = texture(uNormal, vTexCoord).rgb;
	vec3 randVec = normalize(texture(uNoiseMap, vTexCoord * noiseScale).rgb);
	vec3 tangent = normalize(randVec - normal * dot(randVec, normal));
	vec3 bitangent = normalize(cross(tangent, normal));
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.;
	for (int i = 0; i < kernelSize; ++i) {
		// 切线空间->观察空间
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + radius * samplePos;

		// 观察空间->NDC
		vec4 depthTexCoord = (uProjection * vec4(samplePos, 1.0));
		depthTexCoord.xyz /= depthTexCoord.w;
		depthTexCoord.xyz = depthTexCoord.xyz * 0.5 + 0.5;
		float fragDepth = texture(uPosition, depthTexCoord.xy).z;
		float sampleDepth = samplePos.z;
		
		// 检查采样点对应的贴图中像素的深度离片元的深度
		// 如果非常接近（小于radius时），那么rangeCheck永远是1
		// 如果非常远，说明这个片元在物体的边缘处（走很小一段但深度发生了很大的变化）rangeCheck会平滑地过渡到0
		float rangeCheck = smoothstep(0., 1., radius / abs(fragPos.z - fragDepth));

		/********** 注意：现在是在观察空间，越小的值（负得越多）离屏幕越远 **********/
		// fragDepth >= sampleDepth + bias时，采样点不能看见
		// 看不见就是被挡住了，但是如果片元在物体边缘处，会导致所有采样点都被认为可见(全白)或者不可见(全黑)
		occlusion += (fragDepth >= sampleDepth + bias ? 1.0 : 0.0) * rangeCheck;
	}
	// 前面记录的是看不见的比例，SSAO贴图值越大越亮，因此取余值
	occlusion = 1 - (occlusion / kernelSize);
	FragColor = occlusion;
}