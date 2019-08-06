#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inBitangent;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec2 inUV;


layout(location = 0) out vec4 outPos;
layout(location = 1) out vec2 outUv;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outTangent;
layout(location = 4) out vec3 outBitangent;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 cModel;
    mat4 cView;
    mat4 cProj;
	float cFarZ;
	float cNearZ;
};


void main() 
{

	// Positions
    gl_Position = cProj * cView * cModel * vec4(inPosition.xyz, 1.0);
	outPos = cView * cModel * vec4(inPosition.xyz, 1.0);

	// UV co-ordinates.
	outUv = vec2(inUV.x, 1.0 - inUV.y);

	// View-space normal.
	mat3 normalMatrix = transpose(inverse(mat3(cView * cModel)));
	outNormal = normalMatrix * inNormal;
	outTangent = inTangent;
	outBitangent = inBitangent;
}