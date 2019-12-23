#version 330 core

in vec3 FragNormal;
in vec3 FragPos;
in vec2 TextCoord;

uniform samplerCube envText;	// ��������
uniform sampler2D texture_diffuse0;
uniform sampler2D specular_diffuse0;
uniform sampler2D texture_reflection0; // ����map
uniform vec3 cameraPos;

out vec4 color;


void main()
{
	vec4	diffuseColor = texture(texture_diffuse0, TextCoord);
	float	relefctIntensity = texture(texture_reflection0, TextCoord).r; 
	vec4	reflectColor = vec4(0.0, 0.0, 0.0, 0.0);
	if(relefctIntensity > 0.1) // �����Ƿ���reflection map
	{
		vec3 viewDir = normalize(FragPos - cameraPos); // ע�����������ӹ۲���λ��ָ��
		vec3 reflectDir = reflect(viewDir, normalize(FragNormal));
		reflectColor = texture(envText, reflectDir) * relefctIntensity;	// ʹ�÷������������������� ʹ��ǿ��ϵ������
	}
	color = diffuseColor + reflectColor;
}