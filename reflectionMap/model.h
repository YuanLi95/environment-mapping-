#ifndef _MODEL_H_
#define _MODEL_H_

#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "texture.h"

/*
* ����һ��ģ�� ģ�Ϳ��԰���һ������Mesh
*/
class Model
{
public:
	void draw(const Shader& shader) const
	{
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			it->draw(shader);
		}
	}
	bool loadModel(const std::string& filePath)
	{
		Assimp::Importer importer;
		if (filePath.empty())
		{
			std::cerr << "Error:Model::loadModel, empty model file path." << std::endl;
			return false;
		}
		const aiScene* sceneObjPtr = importer.ReadFile(filePath, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
		if (!sceneObjPtr
			|| sceneObjPtr->mFlags == AI_SCENE_FLAGS_INCOMPLETE
			|| !sceneObjPtr->mRootNode)
		{
			std::cerr << "Error:Model::loadModel, description: " 
				<< importer.GetErrorString() << std::endl;
			return false;
		}
		this->modelFileDir = filePath.substr(0, filePath.find_last_of('/'));
		if (!this->processNode(sceneObjPtr->mRootNode, sceneObjPtr))
		{
			std::cerr << "Error:Model::loadModel, process node failed."<< std::endl;
			return false;
		}
		return true;
	}
	~Model()
	{
		for (std::vector<Mesh>::const_iterator it = this->meshes.begin(); this->meshes.end() != it; ++it)
		{
			it->final();
		}
	}
private:
	/*
	* �ݹ鴦��ģ�͵Ľ��
	*/
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr)
	{
		if (!node || !sceneObjPtr)
		{
			return false;
		}
		// �ȴ���������
		for (size_t i = 0; i < node->mNumMeshes; ++i)
		{
			// ע��node�е�mesh�Ƕ�sceneObject��mesh������
			const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]]; 
			if (meshPtr)
			{
				Mesh meshObj;
				if (this->processMesh(meshPtr, sceneObjPtr, meshObj))
				{
					this->meshes.push_back(meshObj);
				}
			}
		}
		// �����ӽ��
		for (size_t i = 0; i < node->mNumChildren; ++i)
		{
			this->processNode(node->mChildren[i], sceneObjPtr);
		}
		return true;
	}
	bool processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr, Mesh& meshObj)
	{
		if (!meshPtr || !sceneObjPtr)
		{
			return false;
		}
		std::vector<Vertex> vertData;
		std::vector<Texture> textures;
		std::vector<GLuint> indices;
		// ��Mesh�õ��������ݡ�����������������
		for (size_t i = 0; i < meshPtr->mNumVertices; ++i)
		{
			Vertex vertex;
			// ��ȡ����λ��
			if (meshPtr->HasPositions())
			{
				vertex.position.x = meshPtr->mVertices[i].x;
				vertex.position.y = meshPtr->mVertices[i].y;
				vertex.position.z = meshPtr->mVertices[i].z;
			}
			// ��ȡ�������� Ŀǰֻ����0������
			if (meshPtr->HasTextureCoords(0))
			{
				vertex.texCoords.x = meshPtr->mTextureCoords[0][i].x;
				vertex.texCoords.y = meshPtr->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.texCoords = glm::vec2(0.0f, 0.0f);
			}
			// ��ȡ����������
			if (meshPtr->HasNormals())
			{
				vertex.normal.x = meshPtr->mNormals[i].x;
				vertex.normal.y = meshPtr->mNormals[i].y;
				vertex.normal.z = meshPtr->mNormals[i].z;
			}
			vertData.push_back(vertex);
		}
		// ��ȡ��������
		for (size_t i = 0; i < meshPtr->mNumFaces; ++i)
		{
			aiFace face = meshPtr->mFaces[i];
			if (face.mNumIndices != 3)
			{
				std::cerr << "Error:Model::processMesh, mesh not transformed to triangle mesh." << std::endl;
				return false;
			}
			for (size_t j = 0; j < face.mNumIndices; ++j)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		// ��ȡ��������
		if (meshPtr->mMaterialIndex >= 0)
		{
			const aiMaterial* materialPtr = sceneObjPtr->mMaterials[meshPtr->mMaterialIndex];
			// ��ȡdiffuse����
			std::vector<Texture> diffuseTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_DIFFUSE, diffuseTexture);
			textures.insert(textures.end(), diffuseTexture.begin(), diffuseTexture.end());
			// ��ȡspecular����
			std::vector<Texture> specularTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_SPECULAR, specularTexture);
			textures.insert(textures.end(), specularTexture.begin(), specularTexture.end());
			// ��ȡReflection ע��: AssImp��Reflection֧�ֲ��� �����������ambient_map
			// ��������Ĵ��� ����Ҫ�޸Ķ�Ӧ��obj�ļ�
			std::vector<Texture> reflectionTexture;
			this->processMaterial(materialPtr, sceneObjPtr, aiTextureType_AMBIENT, reflectionTexture);
			textures.insert(textures.end(), reflectionTexture.begin(), reflectionTexture.end());
		}
		meshObj.setData(vertData, textures, indices);
		return true;
	}
	/*
	* ��ȡһ�������е�����
	*/
	bool processMaterial(const aiMaterial* matPtr, const aiScene* sceneObjPtr, 
		const aiTextureType textureType, std::vector<Texture>& textures)
	{
		textures.clear();

		if (!matPtr 
			|| !sceneObjPtr )
		{
			return false;
		}
		if (matPtr->GetTextureCount(textureType) <= 0)
		{
			return true;
		}
		for (size_t i = 0; i < matPtr->GetTextureCount(textureType); ++i)
		{
			Texture text;
			aiString textPath;
			aiReturn retStatus = matPtr->GetTexture(textureType, i, &textPath);
			if (retStatus != aiReturn_SUCCESS 
				|| textPath.length == 0)
			{
				std::cerr << "Warning, load texture type=" << textureType
					<< "index= " << i << " failed with return value= "
					<< retStatus << std::endl;
				continue;
			}
			std::string absolutePath = this->modelFileDir + "/" + textPath.C_Str();
			LoadedTextMapType::const_iterator it = this->loadedTextureMap.find(absolutePath);
			if (it == this->loadedTextureMap.end()) // ����Ƿ��Ѿ����ع���
			{
				GLuint textId = TextureHelper::load2DTexture(absolutePath.c_str());
				text.id = textId;
				text.path = absolutePath;
				text.type = textureType;
				textures.push_back(text);
				loadedTextureMap[absolutePath] = text;
			}
			else
			{
				textures.push_back(it->second);
			}
		}
		return true;
	}
private:
	std::vector<Mesh> meshes; // ����Mesh
	std::string modelFileDir; // ����ģ���ļ����ļ���·��
	typedef std::map<std::string, Texture> LoadedTextMapType; // key = texture file path
	LoadedTextMapType loadedTextureMap; // �����Ѿ����ص�����
};

#endif