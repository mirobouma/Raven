#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;


layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outFarW;

layout (std140, set=0, binding=0) uniform cbSSAO 
{
	uniform mat4	cProjection;
	uniform mat4	cInvProjection;
	uniform vec4	cFrustumCorners[4];
	uniform float	cNearZ;
	uniform float	cFarZ;
	uniform float	cRandomSize; 
	uniform float	cSampleRadius; 
	uniform float	cIntensity; 
	uniform float	cScale; 
	uniform float	cBias;
	uniform vec4	cKernels[32];
};

void main() 
{
	outFarW = cFrustumCorners[gl_VertexIndex].xyz;
    gl_Position = vec4(inPosition, 1.0, 1.0);
	outUv = vec2(inUV.x, inUV.y);	
}
