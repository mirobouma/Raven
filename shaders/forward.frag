#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 viewDir;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout (set=1, binding=0) uniform texture2D uTex;
layout (set=0, binding=1) uniform texture2D uSSAO;
layout (set=1, binding=2) uniform sampler   uSampler0;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 camPos;
	vec2 screenSize;
} ubo;

void main() 
{
	vec3 lightDir = normalize(vec3(10, 30, 10));
	float lightIntensity = clamp(dot(lightDir, normal), 0, 1);

	vec4 tex = texture(sampler2D(uTex, uSampler0), fragTexCoord);
	float ssao = texture(sampler2D(uSSAO, uSampler0), gl_FragCoord.xy / ubo.screenSize).x;

	const int blurRange = 1;
	int n = 0;
	vec2 texelSize = 1.0 / ubo.screenSize;
	float result = 0.0;

	for (int x = -blurRange; x < blurRange; x++) 
	{
		for (int y = -blurRange; y < blurRange; y++) 
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(sampler2D(uSSAO, uSampler0), (gl_FragCoord.xy / ubo.screenSize) + offset).x;
			n++;
		}
	}
	float ssao2 = result / (float(n));

	vec4 outC = tex * vec4(ssao);
	//vec4 outC = tex * 0.3 * texture(sampler2D(uSSAO, uSampler0), fragTexCoord);

	//if(lightIntensity > 0.0f)
    //{
    //    vec3 reflection = normalize(reflect(normal, -lightDir)); 
    //    float specularI = clamp(pow(clamp(dot(reflection, viewDir), 0.0, 1.0), 42), 0.0, 1.0);
    //    vec4 specular = vec4(specularI, specularI , specularI, 1.0);
	//	
    //    outC = clamp(outC + specular, 0.0, 1.0);
    //}
	
	outColor = clamp(outC, 0.0, 1.0);
	outColor.w = 1.0f;
}