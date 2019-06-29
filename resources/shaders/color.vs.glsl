#version 450
#extension GL_ARB_separate_shader_objects : enable

const int MAX_TEXTURES = 6;

layout(location = 0) in vec3 VertexNormal;
layout(location = 1) in vec3 VertexPosition;
layout(location = 2) in vec2 VertexTextureCoords;

layout(binding = 0) uniform MatrixBuffer {
	mat4 Normal;
	mat4 Model;
	mat4 VP[MAX_TEXTURES];
	mat4 MVP;
} mb;

void main()
{
	gl_Position = (mb.MVP * vec4(VertexPosition, 1.0));
}
