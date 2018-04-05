#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 pos;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;


vec3 getTriPlanarBlend(vec3 _wNorm){
	// in wNorm is the world-space normal of the fragment
	vec3 blending = abs( _wNorm );
	blending = (blending - 0.2) * 7;
	blending = max(blending, 0); // Force weights to sum to 1.0
	float b = (blending.x + blending.y + blending.z);
	blending /= vec3(b, b, b);
	return blending;
}

void main() {
    float normalRepeat = 1.0f;
    float normalScale = 1.0f;
	vec3 lightDir = normalize(vec3(1, 0, 0));
	vec3 blending = getTriPlanarBlend(normal);
    vec3 xaxis = texture(texSampler, pos.yz * normalRepeat).rgb;
    vec3 yaxis = texture(texSampler, pos.zx * normalRepeat).rgb;
    vec3 zaxis = texture(texSampler, pos.xy * normalRepeat).rgb;
    vec3 normalTex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
    //normalTex = normalTex * 2.0 - 1.0;
    //normalTex.xy *= normalScale;
    //normalTex = normalize( normalTex );
    outColor = vec4(normalTex, 1.0);
}