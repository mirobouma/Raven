#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec2 vs_Pos;
layout(location = 1) in vec2 vs_UV;

layout (std140, set=0, binding=0) uniform cbGlobals 
{
	uniform mat4	 cView;
	uniform mat4	 cInvView;
	uniform mat4	 cShadowMat;
	uniform vec4	 cFrustumCorners[4];
	uniform vec4	 cLightDir;
	uniform vec4	 cLightCol;
	uniform vec4	 cLightMieG;
	uniform vec2	 cScreenSize;
	uniform float	 cNearZ;
	uniform float	 cFarZ;
	uniform float	 cAmbientFog;
	uniform float	 cLightIntensity;
	uniform float	 cLightScatteringCoef;
	uniform float	 cLightExtinctionCoef;
	uniform float	 cSkyboxExtinctionCoef;
	uniform uint	 cNumSteps;
	uniform float	 cMaxRayLength;
	uniform bool	 cEnableHeightFog;
	uniform float	 cHeightFog;
	uniform float	 cHeightFogScale;
	uniform float	 cNoiseSpeed;
	uniform float	 cFogDensity;
	uniform bool	 cEnableNoise;
};

layout(location = 0) out vec2 fs_UV;           
layout(location = 1) out vec3 outFarW;

void main ()
{
	fs_UV = vs_UV;
	outFarW = cFrustumCorners[gl_VertexIndex].xyz;
    gl_Position = vec4(vs_Pos, 1.0, 1.0);
}
