#version 330 core

in vec3 TextCoord;
uniform samplerCube  skybox;  // ��sampler2D��ΪsamplerCube

out vec4 color;


void main()
{
	color = texture(skybox, TextCoord);
}