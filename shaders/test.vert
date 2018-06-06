#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inBitangent;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec2 inUV;


layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 viewDir;
layout(location = 3) out vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 camPos;
} ubo;


void main() 
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragTexCoord = vec2(inUV.x, 1 - inUV.y);
	normal = inNormal;
	vec3 viewPos;
	viewPos.x = ubo.view[0][3];
	viewPos.y = ubo.view[1][3];
	viewPos.z = ubo.view[2][3];
	viewDir =  viewPos - (ubo.model * vec4(inPosition, 1.0)).xyz;
}