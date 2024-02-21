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

#define EPS 1e-4 //微小值，用来解决数值精度带来的比较问题
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define SHADOW_MAP_SIZE 2048.0 //阴影贴图尺寸(边长)
#define FRUSTUM_SIZE 200.    //光源视锥体近平面尺寸(边长)

// 随机采样参数
#define NUM_SAMPLES 100 // 采样数
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10 // 泊松圆盘采样参数

#define FILTER_STRIDE 1.0 // 采样步幅

// PCSS所需参数
#define LIGHT_WORLD_SIZE 3   //光源世界尺寸
#define LIGHT_SIZE_UV LIGHT_WORLD_SIZE / FRUSTUM_SIZE //归一化至[0,1]光源窗口 空间后光源尺寸
#define NEAR_PLANE 0.01 // 定义近平面为一个较小值（本来应该也是要使用近平面某个点进行投影计算得到的，但是没必要，反正是估计，减少一点计算量）


// 随机种子函数
// 给1维返1维
highp float rand_1to1(highp float x) {
	// -1 -1
	return fract(sin(x) * 10000.0);
}

// 给2维返一维
highp float rand_2to1(vec2 uv) {
	// 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
	return fract(sin(sn) * c);
}

// 泊松圆盘采样
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


// 自适应计算bias的函数，人为设定c值及采样范围filterRadiusUV可以调整这个值的大小
float getShadowBias(float c, float filterRadiusUV) {
	vec3 normal = normalize(vNormal);
	vec3 lightDir = normalize(uLightPos - vPosition);
	// 同时这个bias与已经设置好的视窗大小和阴影贴图大小有关
	float fragSize = (1. + ceil(filterRadiusUV)) * (FRUSTUM_SIZE / SHADOW_MAP_SIZE / 2.);
	return max(fragSize, fragSize * (1.0 - dot(normal, lightDir))) * c;
}

// 使用阴影贴图计算V项
float useShadowMap(sampler2D shadowMap, vec4 shadowCoord,
	float biasC, float filterRadiusUV) {

	float depth = texture2D(shadowMap, shadowCoord.xy).r;
	float cur_depth = shadowCoord.z;
	// 为了减少自遮挡带来的影响，给每个片元添加一个自适应偏移量
	float bias = getShadowBias(biasC, filterRadiusUV);
	// 01阶跃函数
	if (cur_depth - bias > depth + EPS) {
		return 0.0;
	}
	else {
		return 1.0;
	}
}

float PCF(sampler2D shadowMap, vec4 coords, float biasC, float filterRadiusUV) {
	vec2 uv = coords.xy;
	float cur_depth = coords.z; //当前深度

	// 自适应偏移
	float bias = getShadowBias(biasC, filterRadiusUV);

	// 两种采样方法
	poissonDiskSamples(uv);
	//uniformDiskSamples(uv);

	// 在当前像素周围采样NUM_SAMPLES个像素，计算遮挡百分比
	float count = 0.;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		float ldepth = texture2D(shadowMap, uv + poissonDisk[i] * filterRadiusUV).r;
		count += (ldepth + EPS) > cur_depth - bias ? 1. : 0.;
	}
	// 将百分比作为V项返回
	return count / float(NUM_SAMPLES);
}

float findBlocker(sampler2D shadowMap, vec2 uv, float zReceiver) {
	float posZFromLight = vPositionFromLight.z; // 拿到片元在光源窗口空间中的z值
	// 采样半径由相似三角形计算出，讲道理这里应该除个2，因为是半径
	float searchRadius = LIGHT_SIZE_UV * (posZFromLight - NEAR_PLANE) / posZFromLight;

	float sumOfDepth = 0.;
	int count = 0;
	// 采样计算平均深度
	for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++) {
		float ldepth = texture2D(shadowMap, uv + poissonDisk[i] * searchRadius).r;
		if (ldepth + EPS < zReceiver) {
			sumOfDepth += ldepth;
			count++;
		}
	}
	// 如果没有遮挡物要返回-1，否则count作为除数=0了
	if (count == 0) return -1.;
	return sumOfDepth / float(count);
}

float PCSS(sampler2D shadowMap, vec4 coords, float biasC, float filterRadiusUV) {
	vec2 uv = coords.xy;
	poissonDiskSamples(uv);

	// STEP 1: 计算片元附近平均深度

	float cur_depth = coords.z;
	float avgBlockerDepth = findBlocker(shadowMap, uv, cur_depth);
	// 如果没有遮挡物，V项直接就是1.0
	if (avgBlockerDepth < -EPS) return 1.0;

	// STEP 2: 相似三角形计算伴影直径
	float penumbra = (cur_depth - avgBlockerDepth)* LIGHT_WORLD_SIZE / avgBlockerDepth;

	float bias = getShadowBias(biasC, filterRadiusUV * penumbra);

	// STEP 3: 自适应采样
	float count = 0.;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		float ldepth = texture2D(shadowMap, uv + poissonDisk[i] * penumbra * filterRadiusUV).r;
		count += (ldepth + EPS) > cur_depth - bias ? 1. : 0.;
	}
	// 返回未被遮挡的百分比
	return count / float(NUM_SAMPLES);

}

void main() {
	vec3 shadowCoord = vPositionFromLight.xyz / vPositionFromLight.w;
	// 把[-1,1]的NDC坐标转换为[0,1]的窗口空间坐标
	// 这是因为当初在阴影贴图中保存的z就是窗口空间的坐标
	shadowCoord.xyz = (shadowCoord.xyz + 1.0) / 2.0;
	float depth = texture2D(uShadowMap, shadowCoord.xy).r;
	float visibility;
	float c = 0.4;

	// 无PCF时的Shadow Bias
	float nonePCFBiasC = .04;
	// 有PCF时的Shadow Bias
	float pcfBiasC = .02;

	// PCF的采样范围，因为是在Shadow Map上采样，需要除以Shadow Map大小，得到uv坐标上的范围
	float filterRadiusUV = FILTER_STRIDE / SHADOW_MAP_SIZE;


	// PCSS百分比渐进软阴影
	// visibility = PCSS(uShadowMap, vec4(shadowCoord, 1.0), pcfBiasC, filterRadiusUV);

	// PCF百分比渐进过滤
	 visibility = PCF(uShadowMap, vec4(shadowCoord, 1.0), pcfBiasC, filterRadiusUV);
	// 硬阴影
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