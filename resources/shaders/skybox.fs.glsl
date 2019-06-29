#version 450
#extension GL_ARB_separate_shader_objects : enable

#ifdef GL_FRAGMENT_PRECISION_HIGH
	precision highp float;
#else
	precision mediump float;
#endif

const int MAX_TEXTURES = 6;

layout(location = 0) in      vec3        FragmentTextureCoords;
layout(location = 0) out     vec4        GL_FragColor;
layout(binding  = 2) uniform samplerCube Textures[MAX_TEXTURES];

void main()
{
	GL_FragColor = texture(Textures[0], FragmentTextureCoords);

	// sRGB GAMMA CORRECTION
	float sRGB = (1.0 / 2.2);
	GL_FragColor.rgb = pow(GL_FragColor.rgb, vec3(sRGB, sRGB, sRGB));
}
