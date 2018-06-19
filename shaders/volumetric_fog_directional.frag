#version 450
#extension GL_ARB_separate_shader_objects : enable


layout (std140, set=0, binding=0) uniform cbGlobals 
{
	uniform mat4	 cView;
	uniform mat4	 cInvView;
	uniform mat4	 cShadowMat;
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

layout (set=1, binding=0) uniform texture2D		  uShadowDepth;
layout (set=1, binding=1) uniform texture2D       uSceneDepth;
layout (set=1, binding=2) uniform texture2D		  uScenePosition;
layout (set=1, binding=3) uniform samplerShadow   uShadowSampler;
layout (set=1, binding=4) uniform sampler		  uDepthSampler;


layout(location = 0) in vec2 fs_UV; 
layout(location = 0) out vec4 outColor;

#define PI 3.1415926535897932384626422832795028841971
float doShadow(vec3 viewPos)
{
	vec2 projectTexCoord;
	vec4 lightViewPosition = cShadowMat * (cInvView * vec4(viewPos, 1.0));
		
	// Calculate the projected texture coordinates (Flip Y, since Texture-space, the top is Y 0, whereas in projected space, the top is Y 1.
	projectTexCoord.x =  ((lightViewPosition.x / lightViewPosition.w) / 2.0) + 0.5;
	projectTexCoord.y =  ((lightViewPosition.y / lightViewPosition.w) / 2.0) + 0.5;
		
	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((clamp(projectTexCoord.x, 0.0, 1.0) == projectTexCoord.x) && (clamp(projectTexCoord.y, 0.0, 1.0) == projectTexCoord.y))
	{
		// Calculate the depth of the light.
		float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
		
		const float Dilation = 10.0;
        const float ShadowTexelSize = 1.0 / 2048;
		const float bias = 0.000;
        float d1 = Dilation * ShadowTexelSize * 0.125;

		float shadow = texture(sampler2DShadow(uShadowDepth, uShadowSampler), vec3(projectTexCoord.xy, lightDepthValue + bias));
		
		return shadow * shadow;
	}

	return 1.0;
}

// Apply height fog on the density.
void HeightFog(vec3 wpos, inout float density)
{
	density *= exp(-(-wpos.y + cHeightFog) * cHeightFogScale);
}

// Get the density of the fog at a specified world-space location.
float GetDensity(vec3 worldPos)
{
	float density = cFogDensity;

	// Add height fog, for improved performance this could be a definition instead to avoid branching.
	if(cEnableHeightFog)
	{
		HeightFog(worldPos, density);
	}

	return density;
}

// Get the attenuation at a specified world-space point
float GetAttenuation(vec3 viewPos)
{
	// Check if the point is inside a shadow for a directional light.
	float atten = clamp(doShadow(viewPos) + cAmbientFog, 0.0f, 1.0f);
	return atten;
}

// Mie phase function.
float MieScattering(float cosAngle, vec4 g)
{
	return g.w * (g.x / (pow(g.y - g.z * -cosAngle, 1.5)));
}

const float ditherPattern[4][4] = {{ 0.0f, 0.5f, 0.125f, 0.625f},
								  { 0.75f, 0.22f, 0.875f, 0.375f},
								  { 0.1875f, 0.6875f, 0.0625f, 0.5625},
								  { 0.9375f, 0.4375f, 0.8125f, 0.3125}};


vec4 March(vec3 rayStart, vec3 rayDir, float rayLength)
{
	uint stepCount = cNumSteps;
	float stepSize = rayLength / stepCount;
	vec3 step = rayDir * stepSize;
	vec3 dither = step * ditherPattern[int(fs_UV.x * cScreenSize.x) % 4][int(fs_UV.y * cScreenSize.y) % 4];
	vec3 wPos = rayStart + dither;

	float cosAngle = dot(transpose(inverse(mat3(cView))) * normalize(-cLightDir.xyz), -rayDir);
	vec4 vLight = vec4(0.0, 0.0, 0.0, 0.0);

	// Density between the camera and the first step.
	float extinction = length(-wPos) * cLightExtinctionCoef * 0.5;

	for (uint i = 0; i < stepCount; ++i)
	{
		float atten = GetAttenuation(wPos);
		float density = GetDensity(wPos);

		float scattering = cLightScatteringCoef * stepSize * density;
		extinction += cLightExtinctionCoef * stepSize * density;

		vec4 light = vec4(atten * scattering * exp(-extinction));

		// Add the light for this step to the culumative light.
		vLight += light;

		// Step forward on the ray.
		wPos += step;	
	}

	// Apply mie scattering to the light.
	vLight *= MieScattering(cosAngle, cLightMieG);

	// Apply light color to the light.
	vLight *= cLightCol *cLightIntensity;

	// Make sure we don't substract light, since it's only addative.
	vLight = max(vec4(0.0), vLight);

	// out-scattering/absorption for directional light.
	vLight.w = exp(-extinction);

	return vLight;
}


void main()
{
	
	// Sample depth texture
	float depth = texture(sampler2D(uSceneDepth, uDepthSampler), fs_UV).x;
	vec4 positiondepth = texture(sampler2D(uScenePosition, uDepthSampler), fs_UV);

	vec3 position = positiondepth.xyz;
	float linearDepth = positiondepth.z;
	 
	// Calculate the ray from the camera to the far plane and multiply it by linear depth (0-1)
	vec3 vpos = position;
	vec3 rayStart = vec3(0.0, 0.0, 0.0);
	vec3 rayDir = vpos - rayStart;				
	//rayDir *= linearDepth;

	// get the length of the ray from the camera to the furthest geometry and normalize the direction afterwards.
	float rayLength = length(rayDir);
	rayDir /= rayLength;

	// Clamp to the maximum ray length.
	rayLength = min(rayLength, cMaxRayLength);


	vec4 color = March(rayStart, rayDir, rayLength);

	// If we didn't have any geometry rendered to the depth buffer, lerp extinction to the skybox's extinction value.
	if (linearDepth > 0.999999)
	{
		color.w = mix(color.w, 1, cSkyboxExtinctionCoef);
	}

    outColor = color;
}
