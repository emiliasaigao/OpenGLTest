#version 330 core

out float FragColor;

in vec2 vTexCoord;

// λ�á���ȡ�������ͼ���ǻ��ڹ۲�ռ��
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
		// ���߿ռ�->�۲�ռ�
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + radius * samplePos;

		// �۲�ռ�->NDC
		vec4 depthTexCoord = (uProjection * vec4(samplePos, 1.0));
		depthTexCoord.xyz /= depthTexCoord.w;
		depthTexCoord.xyz = depthTexCoord.xyz * 0.5 + 0.5;
		float fragDepth = texture(uPosition, depthTexCoord.xy).z;
		float sampleDepth = samplePos.z;
		
		// ���������Ӧ����ͼ�����ص������ƬԪ�����
		// ����ǳ��ӽ���С��radiusʱ������ôrangeCheck��Զ��1
		// ����ǳ�Զ��˵�����ƬԪ������ı�Ե�����ߺ�Сһ�ε���ȷ����˺ܴ�ı仯��rangeCheck��ƽ���ع��ɵ�0
		float rangeCheck = smoothstep(0., 1., radius / abs(fragPos.z - fragDepth));

		/********** ע�⣺�������ڹ۲�ռ䣬ԽС��ֵ������Խ�ࣩ����ĻԽԶ **********/
		// fragDepth >= sampleDepth + biasʱ�������㲻�ܿ���
		// ���������Ǳ���ס�ˣ��������ƬԪ�������Ե�����ᵼ�����в����㶼����Ϊ�ɼ�(ȫ��)���߲��ɼ�(ȫ��)
		occlusion += (fragDepth >= sampleDepth + bias ? 1.0 : 0.0) * rangeCheck;
	}
	// ǰ���¼���ǿ������ı�����SSAO��ͼֵԽ��Խ�������ȡ��ֵ
	occlusion = 1 - (occlusion / kernelSize);
	FragColor = occlusion;
}