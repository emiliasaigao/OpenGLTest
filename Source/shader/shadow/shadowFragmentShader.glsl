#version 330 core

in vec4 vPositionFromLight;
in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;
out vec4 FragColor;

uniform vec3 uLightPos;

struct Meterial {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_ambient1;
};

uniform Meterial meterial;
uniform sampler2D uShadowMap;

#define EPS 1e-4 //΢Сֵ�����������ֵ���ȴ����ıȽ�����
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define SHADOW_MAP_SIZE 2048.0 //��Ӱ��ͼ�ߴ�(�߳�)
#define FRUSTUM_SIZE 200.    //��Դ��׶���ƽ��ߴ�(�߳�)

// �����������
#define NUM_SAMPLES 100 // ������
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10 // ����Բ�̲�������

#define FILTER_STRIDE 1.0 // ��������

// PCSS�������
#define LIGHT_WORLD_SIZE 3   //��Դ����ߴ�
#define LIGHT_SIZE_UV LIGHT_WORLD_SIZE / FRUSTUM_SIZE //��һ����[0,1]��Դ���� �ռ���Դ�ߴ�
#define NEAR_PLANE 0.01 // �����ƽ��Ϊһ����Сֵ������Ӧ��Ҳ��Ҫʹ�ý�ƽ��ĳ�������ͶӰ����õ��ģ�����û��Ҫ�������ǹ��ƣ�����һ���������


// ������Ӻ���
// ��1ά��1ά
highp float rand_1to1(highp float x) {
	// -1 -1
	return fract(sin(x) * 10000.0);
}

// ��2ά��һά
highp float rand_2to1(vec2 uv) {
	// 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
	return fract(sin(sn) * c);
}

// ����Բ�̲���
vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples(const in vec2 randomSeed) {

	float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(NUM_SAMPLES);
	float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

	float angle = rand_2to1(randomSeed) * PI2;
	float radius = INV_NUM_SAMPLES;
	float radiusStep = radius;

	for (int i = 0; i < NUM_SAMPLES; i++) {
		poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
		radius += radiusStep;
		angle += ANGLE_STEP;
	}
}


// ����Ӧ����bias�ĺ�������Ϊ�趨cֵ��������ΧfilterRadiusUV���Ե������ֵ�Ĵ�С
float getShadowBias(float c, float filterRadiusUV) {
	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(uLightPos - vPosition);
	// ͬʱ���bias���Ѿ����úõ��Ӵ���С����Ӱ��ͼ��С�й�
	float fragSize = (1. + ceil(filterRadiusUV)) * (FRUSTUM_SIZE / SHADOW_MAP_SIZE / 2.);
	return max(fragSize, fragSize * (1.0 - dot(normal, lightDir))) * c;
}

// ʹ����Ӱ��ͼ����V��
float useShadowMap(sampler2D shadowMap, vec4 shadowCoord,
	float biasC, float filterRadiusUV) {

	float depth = texture2D(shadowMap, shadowCoord.xy).r;
	float cur_depth = shadowCoord.z;
	// Ϊ�˼������ڵ�������Ӱ�죬��ÿ��ƬԪ���һ������Ӧƫ����
	float bias = getShadowBias(biasC, filterRadiusUV);
	// 01��Ծ����
	if (cur_depth - bias > depth + EPS) {
		return 0.0;
	}
	else {
		return 1.0;
	}
}

float PCF(sampler2D shadowMap, vec4 coords, float biasC, float filterRadiusUV) {
	vec2 uv = coords.xy;
	float cur_depth = coords.z; //��ǰ���

	// ����Ӧƫ��
	float bias = getShadowBias(biasC, filterRadiusUV);

	// ���ֲ�������
	poissonDiskSamples(uv);
	//uniformDiskSamples(uv);

	// �ڵ�ǰ������Χ����NUM_SAMPLES�����أ������ڵ��ٷֱ�
	float count = 0.;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		float ldepth = texture2D(shadowMap, uv + poissonDisk[i] * filterRadiusUV).r;
		count += (ldepth + EPS) > cur_depth - bias ? 1. : 0.;
	}
	// ���ٷֱ���ΪV���
	return count / float(NUM_SAMPLES);
}

float findBlocker(sampler2D shadowMap, vec2 uv, float zReceiver) {
	float posZFromLight = vPositionFromLight.z; // �õ�ƬԪ�ڹ�Դ���ڿռ��е�zֵ
	// �����뾶�����������μ����������������Ӧ�ó���2����Ϊ�ǰ뾶
	float searchRadius = LIGHT_SIZE_UV * (posZFromLight - NEAR_PLANE) / posZFromLight;

	float sumOfDepth = 0.;
	int count = 0;
	// ��������ƽ�����
	for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++) {
		float ldepth = texture2D(shadowMap, uv + poissonDisk[i] * searchRadius).r;
		if (ldepth + EPS < zReceiver) {
			sumOfDepth += ldepth;
			count++;
		}
	}
	// ���û���ڵ���Ҫ����-1������count��Ϊ����=0��
	if (count == 0) return -1.;
	return sumOfDepth / float(count);
}

float PCSS(sampler2D shadowMap, vec4 coords, float biasC, float filterRadiusUV) {
	vec2 uv = coords.xy;
	poissonDiskSamples(uv);

	// STEP 1: ����ƬԪ����ƽ�����

	float cur_depth = coords.z;
	float avgBlockerDepth = findBlocker(shadowMap, uv, cur_depth);
	// ���û���ڵ��V��ֱ�Ӿ���1.0
	if (avgBlockerDepth < -EPS) return 1.0;

	// STEP 2: ���������μ����Ӱֱ��
	float penumbra = (cur_depth - avgBlockerDepth)* LIGHT_WORLD_SIZE / avgBlockerDepth;

	float bias = getShadowBias(biasC, filterRadiusUV * penumbra);

	// STEP 3: ����Ӧ����
	float count = 0.;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		float ldepth = texture2D(shadowMap, uv + poissonDisk[i] * penumbra * filterRadiusUV).r;
		count += (ldepth + EPS) > cur_depth - bias ? 1. : 0.;
	}
	// ����δ���ڵ��İٷֱ�
	return count / float(NUM_SAMPLES);

}

void main() {
	vec3 shadowCoord = vPositionFromLight.xyz / vPositionFromLight.w;
	// ��[-1,1]��NDC����ת��Ϊ[0,1]�Ĵ��ڿռ�����
	// ������Ϊ��������Ӱ��ͼ�б����z���Ǵ��ڿռ������
	shadowCoord.xyz = (shadowCoord.xyz + 1.0) / 2.0;
	float depth = texture2D(uShadowMap, shadowCoord.xy).r;
	float visibility;
	float c = 0.4;

	// ��PCFʱ��Shadow Bias
	float nonePCFBiasC = .04;
	// ��PCFʱ��Shadow Bias
	float pcfBiasC = .02;

	// PCF�Ĳ�����Χ����Ϊ����Shadow Map�ϲ�������Ҫ����Shadow Map��С���õ�uv�����ϵķ�Χ
	float filterRadiusUV = FILTER_STRIDE / SHADOW_MAP_SIZE;


	// PCSS�ٷֱȽ�������Ӱ
	// visibility = PCSS(uShadowMap, vec4(shadowCoord, 1.0), pcfBiasC, filterRadiusUV);

	// PCF�ٷֱȽ�������
	 visibility = PCF(uShadowMap, vec4(shadowCoord, 1.0), pcfBiasC, filterRadiusUV);
	// Ӳ��Ӱ
	//visibility = useShadowMap(uShadowMap, vec4(shadowCoord, 1.0), c, 0.);

	vec3 color = 0.2 * (texture(meterial.texture_diffuse1, vTexCoord).rgb +
		texture(meterial.texture_specular1, vTexCoord).rgb +
		texture(meterial.texture_ambient1, vTexCoord).rgb);
	
	vec3 baseColor = 0.05 * (texture(meterial.texture_diffuse1, vTexCoord).rgb +
		texture(meterial.texture_specular1, vTexCoord).rgb +
		texture(meterial.texture_ambient1, vTexCoord).rgb);
	FragColor = vec4(visibility * color + baseColor, 1.0);
	//FragColor = vec4(vec3(depth), 1.0);
}