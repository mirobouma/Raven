#version 450
#extension GL_ARB_separate_shader_objects : enable

// Implementation from https://www.gamedev.net/articles/programming/graphics/a-simple-and-practical-approach-to-ssao-r2753/ converted to GLSL.

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec3 in_farW;

layout(location = 0) out float outColor;

layout (set=1, binding=0) uniform texture2D uTexDepth;
layout (set=1, binding=1) uniform texture2D uTexRandom;

layout (set=1, binding=2) uniform sampler uDepthSampler;
layout (set=1, binding=3) uniform sampler uRandomSampler;

layout (std140, set=0, binding=0) uniform cbSSAO 
{
	uniform mat4	cProjection;
	uniform mat4	cInvProjection;
	uniform vec4	cFrustumCorners[4];
	uniform vec2	cScreenSize;
	uniform float	cNearZ;
	uniform float	cFarZ;
	uniform float	cRandomSize; 
	uniform float	cSampleRadius; 
	uniform float	cIntensity; 
	uniform float	cScale; 
	uniform float	cBias;
	uniform float	cPad0;
	uniform vec4	cKernels[32];
};

float linearizeDepth(in float depth)
{
	float c1 = cFarZ / cNearZ;
    float c0 = 1.0 - c1;
	float linearDepth = 1.0 / (c0 * depth + c1);

	return linearDepth;
}

vec3 getPosition(in vec2 uv) 
{
	float depth = texture(sampler2D(uTexDepth, uDepthSampler), uv).x;
	float linearDepth = linearizeDepth(depth);

	// view space
	return in_farW * linearDepth;
}

vec3 getNormal(in vec2 uv) 
{
	const vec2 offset1 = vec2(0.0, 1.0 / cScreenSize.y);
	const vec2 offset2 = vec2(1.0 / cScreenSize.x,0.0);

	float depth = linearizeDepth(texture(sampler2D(uTexDepth, uDepthSampler), uv).x);

	float depth1 = linearizeDepth(texture(sampler2D(uTexDepth, uDepthSampler), uv + offset1).x);
	float depth2 = linearizeDepth(texture(sampler2D(uTexDepth, uDepthSampler), uv + offset2).x);
	
	vec3 p1 = vec3(offset1, depth1 - depth);
	vec3 p2 = vec3(offset2, depth2 - depth);
	
	vec3 normal = cross(p1, p2);
	return normalize(normal);
}

vec3 getRandom(in vec2 uv) 
{
  return normalize(texture(sampler2D(uTexRandom, uRandomSampler), cScreenSize * uv / cRandomSize).xyz * 2.0f - 1.0f); 
}

const vec3 points[] =
{
		vec3(-0.134, 0.044, -0.825),
		vec3(0.045, -0.431, -0.529),
		vec3(-0.537, 0.195, -0.371),
		vec3(0.525, -0.397, 0.713),
		vec3(0.895, 0.302, 0.139),
		vec3(-0.613, -0.408, -0.141),
		vec3(0.307, 0.822, 0.169),
		vec3(-0.819, 0.037, -0.388),
		vec3(0.376, 0.009, 0.193),
		vec3(-0.006, -0.103, -0.035),
		vec3(0.098, 0.393, 0.019),
		vec3(0.542, -0.218, -0.593),
		vec3(0.526, -0.183, 0.424),
		vec3(-0.529, -0.178, 0.684),
		vec3(0.066, -0.657, -0.570),
		vec3(-0.214, 0.288, 0.188),
		vec3(-0.689, -0.222, -0.192),
		vec3(-0.008, -0.212, -0.721),
		vec3(0.053, -0.863, 0.054),
		vec3(0.639, -0.558, 0.289),
		vec3(-0.255, 0.958, 0.099),
		vec3(-0.488, 0.473, -0.381),
		vec3(-0.592, -0.332, 0.137),
		vec3(0.080, 0.756, -0.494),
		vec3(-0.638, 0.319, 0.686),
		vec3(-0.663, 0.230, -0.634),
		vec3(0.235, -0.547, 0.664),
		vec3(0.164, -0.710, 0.086),
		vec3(-0.009, 0.493, -0.038),
		vec3(-0.322, 0.147, -0.105),
		vec3(-0.554, -0.725, 0.289),
		vec3(0.534, 0.157, -0.250),
};

void main() 
{
	// Get G-Buffer values
	vec3 fragPos = getPosition(in_uv);
	vec3 normal = getNormal(in_uv);

	// Get a random vector using a noise lookup
	vec3 randomVec = getRandom(in_uv);
	
	// Create TBN matrix
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(tangent, normal);
	mat3 TBN = mat3(tangent, bitangent, normal);

	// Calculate occlusion value
	float occlusion = 0.0f;
	float steps = 0.0f;

	for(int i = 0; i < 16; i++)
	{		
		vec3 samplePos = TBN * points[i].xyz; 
		samplePos = fragPos + (samplePos * cSampleRadius); 
		
		// project
		vec4 offset = vec4(samplePos, 1.0f);
		offset = cProjection * offset; 
		offset.xyz /= offset.w; 
		offset.xyz = offset.xyz * 0.5f + 0.5f; 
		
		float sampleDepth = getPosition(offset.xy).z; 

		// Range check
		float rangeCheck = smoothstep(0.0, 1.0, cSampleRadius / abs(fragPos.z - sampleDepth ));
		occlusion += (sampleDepth >= samplePos.z + cBias ? 1.0 : 0.0) * rangeCheck;

		//float rangeCheck = smoothstep(0.0f, 1.0f, abs(fragPos.z - sampleDepth) / cSampleRadius);
		//occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f) * rangeCheck;        
		//occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f);  
		steps += rangeCheck;
	}
	occlusion = 1.0 - ((occlusion / float(steps))*cIntensity);
	
	outColor = occlusion;
}