#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec2 inUv;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outAlbedo;
layout(location = 2) out vec4 outNormal;

layout (set=1, binding=0) uniform texture2D uAlbedoTexture;
layout (set=1, binding=1) uniform texture2D uNormalTexture;
layout (set=1, binding=2) uniform texture2D uMetallicTexture;
layout (set=1, binding=3) uniform texture2D uRoughnessTexture;

layout (set=1, binding=4) uniform sampler   uSampler0;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 cModel;
    mat4 cView;
    mat4 cProj;
	float cFarZ;
	float cNearZ;
};

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f; 
	return (2.0f * cNearZ * cFarZ) / (cFarZ + cNearZ - z * (cFarZ - cNearZ));	
}

void main() 
{
	vec4 tex = texture(sampler2D(uAlbedoTexture, uSampler0), inUv);
	vec3 normal = normalize(texture(sampler2D(uNormalTexture, uSampler0), inUv).xyz * 2.0 - 1.0);

	mat3 TBN = mat3(inTangent, inBitangent, inNormal);

	outPosition = vec4(inPos.xyz, linearDepth(gl_FragCoord.z));
	outNormal = vec4(normalize(TBN * normal) * 0.5 + 0.5, 1.0);
	outAlbedo = vec4(tex.rgb, 1.0);
}