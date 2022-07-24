#pragma once
#include "Mesh.h"
#include "../Utils/SimpleMath.h"

using namespace DirectX;

class Model
{
public:
	void Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor);
	void Draw(Matrix4x4 worldMatrix, Matrix4x4& viewProjectionMatrix);

private:
	std::vector<Mesh> meshes;
	aiColor3D defaultColor;
	void Load(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene, Matrix4x4 parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Matrix4x4 transformMatrix);
	int GetTextureIndex(aiString* pStr);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<VSConstantBuffer>* vsConstantBuffer = nullptr;
	std::string directory = "";
};