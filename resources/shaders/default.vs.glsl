#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 VertexNormal;
layout(location = 1) in vec3 VertexPosition;
layout(location = 2) in vec2 VertexTextureCoords;

layout(location = 0) out vec3 FragmentNormal;
layout(location = 1) out vec4 FragmentPosition;
layout(location = 2) out vec2 FragmentTextureCoords;
layout(location = 3) out vec4 ClipSpace;

layout(binding = 0) uniform MatrixBuffer {
	mat4 Normal;
	mat4 Model;
	mat4 View;
	mat4 Projection;
	mat4 MVP;
} mb;

void main()
{
	// TODO: Calculate the normal matrix on the CPU and send it to the shaders via a uniform before drawing
	//FragmentNormal = vec3(mb.Model * vec4(VertexNormal, 0.0));
	//FragmentNormal = vec3(transpose(inverse(mat3(mb.Model))) * VertexNormal);
	FragmentNormal        = (mat3(mb.Normal) * VertexNormal);
	FragmentPosition      = (mb.Model * vec4(VertexPosition, 1.0));
	FragmentTextureCoords = VertexTextureCoords;
	ClipSpace             = (mb.MVP * vec4(VertexPosition, 1.0));
	gl_Position           = ClipSpace;
}
