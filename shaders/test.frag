#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 viewDir;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout (set=1, binding=0) uniform texture2D uTex;
layout (set=1, binding=1) uniform sampler   uSampler0;

void main() 
{
	vec3 lightDir = normalize(vec3(10, 30, 10));
	float lightIntensity = clamp(dot(lightDir, normal), 0, 1);


	if(lightIntensity > 0.0f)
    {
        vec3 reflection = normalize(reflect(normal, -lightDir)); 
        float specularI = clamp(pow(clamp(dot(reflection, viewDir), 0.0, 1.0), 42), 0.0, 1.0);
        vec4 specular = vec4(specularI, specularI , specularI, 1.0);
		
        outColor = clamp(outColor + specular, 0.0, 1.0);
    }
	
	vec4 tex = texture(sampler2D(uTex, uSampler0), fragTexCoord);
	outColor = clamp(tex, 0.0, 1.0);

}