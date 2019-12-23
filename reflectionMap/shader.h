#ifndef _SHADER_H_
#define _SHADER_H_

#include <GLEW/glew.h>
#include <iterator>     // std::istreambuf_iterator
#include <string>       
#include <vector>
#include <iostream>
#include <fstream>

struct ShaderFile
{
	GLenum shaderType;
	const char* filePath;
	ShaderFile(GLenum type, const char* path) 
		:shaderType(type), filePath(path){}
};

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragPath) :programId(0)
	{
		std::vector<ShaderFile> fileVec;
		fileVec.push_back(ShaderFile(GL_VERTEX_SHADER, vertexPath));
		fileVec.push_back(ShaderFile(GL_FRAGMENT_SHADER, fragPath));
		loadFromFile(fileVec);
	}
	Shader(const char* vertexPath, const char* fragPath, const char* geometryPath) :programId(0)
	{
		std::vector<ShaderFile> fileVec;
		fileVec.push_back(ShaderFile(GL_VERTEX_SHADER, vertexPath));
		fileVec.push_back(ShaderFile(GL_FRAGMENT_SHADER, fragPath));
		fileVec.push_back(ShaderFile(GL_GEOMETRY_SHADER, geometryPath));
		loadFromFile(fileVec);
	}
	void use() const
	{
		glUseProgram(this->programId);
	}
	~Shader()
	{
		if (this->programId)
		{
			glDeleteProgram(this->programId);
		}
	}
public:
	GLuint programId;
private:
	/*
	* ���ļ����ض����ƬԪ��ɫ��
	* ���ݲ���Ϊ [(��ɫ���ļ����ͣ���ɫ���ļ�·��)+]
	*/
	void loadFromFile(std::vector<ShaderFile>& shaderFileVec)
	{
		std::vector<GLuint> shaderObjectIdVec;
		std::string vertexSource, fragSource;
		std::vector<std::string> sourceVec;
		size_t shaderCount = shaderFileVec.size();
		// ��ȡ�ļ�Դ����
		for (size_t i = 0; i < shaderCount; ++i)
		{
			std::string shaderSource;
			if (!loadShaderSource(shaderFileVec[i].filePath, shaderSource))
			{
				std::cout << "Error::Shader could not load file:" << shaderFileVec[i].filePath << std::endl;
				return;
			}
			sourceVec.push_back(shaderSource);
		}
		bool bSuccess = true;
		// ����shader object
		for (size_t i = 0; i < shaderCount; ++i)
		{
			GLuint shaderId = glCreateShader(shaderFileVec[i].shaderType);
			const char *c_str = sourceVec[i].c_str();
			glShaderSource(shaderId, 1, &c_str, NULL);
			glCompileShader(shaderId);
			GLint compileStatus = 0;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus); // ������״̬
			if (compileStatus == GL_FALSE) // ��ȡ���󱨸�
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errLog(maxLength);
				glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errLog[0]);
				std::cout << "Error::Shader file [" << shaderFileVec[i].filePath << " ] compiled failed,"
						  << &errLog[0] << std::endl;
				bSuccess = false;
			}
			shaderObjectIdVec.push_back(shaderId);
		}
		// ����shader program
		if (bSuccess)
		{
			this->programId = glCreateProgram();
			for (size_t i = 0; i < shaderCount; ++i)
			{
				glAttachShader(this->programId, shaderObjectIdVec[i]);
			}
			glLinkProgram(this->programId);
			GLint linkStatus;
			glGetProgramiv(this->programId, GL_LINK_STATUS, &linkStatus);
			if (linkStatus == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(this->programId, GL_INFO_LOG_LENGTH, &maxLength);
				std::vector<GLchar> errLog(maxLength);
				glGetProgramInfoLog(this->programId, maxLength, &maxLength, &errLog[0]);
				std::cout << "Error::shader link failed," << &errLog[0] << std::endl;
			}
		}
		// ������ɺ�detach ���ͷ�shader object
		for (size_t i = 0; i < shaderCount; ++i)
		{
			if (this->programId != 0)
			{
				glDetachShader(this->programId, shaderObjectIdVec[i]);
			}
			glDeleteShader(shaderObjectIdVec[i]);
		}
	}
	/*
	* ��ȡ��ɫ������Դ��
	*/
	bool loadShaderSource(const char* filePath,std::string& source)
	{
		source.clear();
		std::ifstream in_stream(filePath);
		if (!in_stream)
		{
			return false;
		}
		source.assign(std::istreambuf_iterator<char>(in_stream),
			std::istreambuf_iterator<char>()); // �ļ��������������ַ���
		return true;
	}
};
#endif