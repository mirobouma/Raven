#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler;


void main() {
	vec3 lightDir = normalize(vec3(1, 0, 0));
    outColor = texture(texSampler, 0.25 + fragTexCoord * 0.25f) * dot(normal, lightDir);
}