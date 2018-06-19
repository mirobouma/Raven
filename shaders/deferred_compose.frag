#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout (set=1, binding=0) uniform texture2D uTexAlbedo;
layout (set=1, binding=1) uniform texture2D uTexPosition;
layout (set=1, binding=2) uniform texture2D uTexNormal;
layout (set=1, binding=3) uniform texture2D uTexSSAO;
layout (set=1, binding=4) uniform texture2D uTexShadow;
layout (set=1, binding=5) uniform texture2D uTexVolumetricFog;

layout (set=1, binding=6) uniform sampler			uSampler0;
layout (set=1, binding=7) uniform samplerShadow		uShadowSampler;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 cView;
    mat4 cProj;
	mat4 cInvView;
	mat4 cShadowMatrix;
	vec4 cLightDir;

	float cFarZ;
	float cNearZ;
	vec2 cScreenSize;
};

float doShadow(vec3 viewPos)
{
	vec2 projectTexCoord;
	vec4 lightViewPosition = cShadowMatrix * (cInvView * vec4(viewPos, 1.0));
		
	// Calculate the projected texture coordinates (Flip Y, since Texture-space, the top is Y 0, whereas in projected space, the top is Y 1.
	projectTexCoord.x =  ((lightViewPosition.x / lightViewPosition.w) / 2.0) + 0.5;
	projectTexCoord.y =  ((lightViewPosition.y / lightViewPosition.w) / 2.0) + 0.5;
		
	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((clamp(projectTexCoord.x, 0.0, 1.0) == projectTexCoord.x) && (clamp(projectTexCoord.y, 0.0, 1.0) == projectTexCoord.y))
	{
		// Calculate the depth of the light.
		float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
			
        const float Dilation = 1.5;
        const float ShadowTexelSize = 1.0 / 2048;
		const float bias = -0.000;

        // Sample around the texel and blend these results together for smooth shadows (PCF).
        float d1 = Dilation * ShadowTexelSize * 0.125;
        float d2 = Dilation * ShadowTexelSize * 0.875;
        float d3 = Dilation * ShadowTexelSize * 0.625;
        float d4 = Dilation * ShadowTexelSize * 0.375;

		float shadow = (
			2.0 * texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy, lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2(-d2,  d1), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2(-d1, -d2), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2( d2, -d1), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2( d1,  d2), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2(-d4,  d3), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2(-d3, -d4), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2( d4, -d3), lightDepthValue + bias)) +
			texture(sampler2DShadow(uTexShadow, uShadowSampler), vec3(projectTexCoord.xy + vec2( d3,  d4), lightDepthValue + bias))
			) / 10.0;
		
		return shadow * shadow;
	}

	return 1.0;
}

void main() 
{
	vec4 tex = texture(sampler2D(uTexAlbedo, uSampler0), fragTexCoord);
	float ssao = texture(sampler2D(uTexSSAO, uSampler0), fragTexCoord).r;
	vec3 normal = normalize(texture(sampler2D(uTexNormal, uSampler0), fragTexCoord).xyz * 2.0 - 1.0);
	vec3 position = texture(sampler2D(uTexPosition, uSampler0), fragTexCoord).xyz;
	vec4 fog = texture(sampler2D(uTexVolumetricFog, uSampler0), fragTexCoord);

	vec3 L = transpose(inverse(mat3(cView))) * normalize(cLightDir.xyz);
	float NdotL = max(0.0, dot(normal, -L));

	const int blurRange = 2;
	int n = 0;
	vec2 texelSize = 1.0 / cScreenSize;
	float result = 0.0;

	for (int x = -blurRange; x < blurRange; x++) 
	{
		for (int y = -blurRange; y < blurRange; y++) 
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(sampler2D(uTexSSAO, uSampler0), fragTexCoord + offset).x;
			n++;
		}
	}
	float ssao2 = result / (float(n));

	outColor = clamp(tex * clamp(clamp(doShadow(position) * NdotL, 0.0, 1.0), 0.2, 1.0) * ssao2.rrrr + vec4(fog.xyz * fog.w, 0.0), 0.0, 1.0);
	outColor.w = 1.0f;
}