#version 330 core

in vec3 FragNormal;
in vec3 FragPos;

uniform samplerCube envText; // ��������
uniform vec3 cameraPos;

out vec4 color;


void main()
{
	float indexRation = 1.00 / 1.52;
	vec3 viewDir = normalize(FragPos - cameraPos); // ע�����������ӹ۲���λ��ָ��
	vec3 refractDir = refract(viewDir, normalize(FragNormal), indexRation);
	color = texture(envText, refractDir);	// ʹ�� �������� ������������
}