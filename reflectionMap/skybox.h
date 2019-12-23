#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_

#include <vector>
#include "texture.h"
/*
* ��cubeʵ�ֵ���հ�Χ��
*/
class SkyBox
{
public:
	SkyBox() : skyBoxTextId(0){}
	bool init(std::vector<const char*> picFilePathVec,
		GLint internalFormat = GL_RGB, GLenum picFormat = GL_RGB,
		GLenum picDataType = GL_UNSIGNED_BYTE, int loadChannels = SOIL_LOAD_RGB)
	{
		this->setupData();
		this->skyBoxTextId = TextureHelper::loadCubeMapTexture(picFilePathVec,
			internalFormat, picFormat, picDataType, loadChannels);
		return this->skyBoxTextId != 0;
	}
	void draw(const Shader& skyBoxShader) const
	{
		GLint OldDepthFuncMode;
		glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

		glDepthFunc(GL_LEQUAL);
		skyBoxShader.use();
		glBindVertexArray(this->skyBoxVAOId);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glDepthFunc(OldDepthFuncMode);
	}
	~SkyBox()
	{
		glDeleteVertexArrays(1, &this->skyBoxVAOId);
		glDeleteBuffers(1, &this->skyBoxVBOId);
	}
	GLuint getTextId() const
	{
		return this->skyBoxTextId;
	}
private:
	GLuint skyBoxTextId;
	GLuint skyBoxVAOId, skyBoxVBOId;
private:
	void setupData()
	{
		// ָ����Χ�еĶ������� λ��
		GLfloat skyboxVertices[] = {
			// ����
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, -1.0f, -1.0f,	// B
			1.0f, -1.0f, -1.0f,		// C
			1.0f, -1.0f, -1.0f,		// C
			1.0f, 1.0f, -1.0f,		// D
			-1.0f, 1.0f, -1.0f,		// A

			// �����
			-1.0f, -1.0f, 1.0f,		// E
			-1.0f, -1.0f, -1.0f,	// B
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, 1.0f, -1.0f,		// A
			-1.0f, 1.0f, 1.0f,		// F
			-1.0f, -1.0f, 1.0f,		// E

			// �Ҳ���
			1.0f, -1.0f, -1.0f,		// C
			1.0f, -1.0f, 1.0f,		// G
			1.0f, 1.0f, 1.0f,		// H
			1.0f, 1.0f, 1.0f,		// H
			1.0f, 1.0f, -1.0f,		// D
			1.0f, -1.0f, -1.0f,		// C

			// ����
			-1.0f, -1.0f, 1.0f,  // E
			-1.0f, 1.0f, 1.0f,  // F
			1.0f, 1.0f, 1.0f,  // H
			1.0f, 1.0f, 1.0f,  // H
			1.0f, -1.0f, 1.0f,  // G
			-1.0f, -1.0f, 1.0f,  // E

			// ����
			-1.0f, 1.0f, -1.0f,  // A
			1.0f, 1.0f, -1.0f,  // D
			1.0f, 1.0f, 1.0f,  // H
			1.0f, 1.0f, 1.0f,  // H
			-1.0f, 1.0f, 1.0f,  // F
			-1.0f, 1.0f, -1.0f,  // A

			// ����
			-1.0f, -1.0f, -1.0f,  // B
			-1.0f, -1.0f, 1.0f,   // E
			1.0f, -1.0f, 1.0f,    // G
			1.0f, -1.0f, 1.0f,    // G
			1.0f, -1.0f, -1.0f,   // C
			-1.0f, -1.0f, -1.0f,  // B
		};
		glGenVertexArrays(1, &this->skyBoxVAOId);
		glGenBuffers(1, &this->skyBoxVBOId);
		glBindVertexArray(this->skyBoxVAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->skyBoxVBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		// ����λ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			3 * sizeof(GL_FLOAT), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
};

#endif