#pragma once
#include "Mesh.h"
#include "../Components.h"
#include "../Utils/SimpleMath.h"

class Model:public Components
{
public:
	//vector<aiMesh*> meshs
	void Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor);
	void Draw(Matrix4x4 worldMatrix, Matrix4x4& viewProjectionMatrix);

	vector<Mesh> m_meshes;

	/* GJK를 위한 함수 */
	Vector3 GetFarthestVertexOfDirectionWorldCoord(Vector3 direction);

private:
	aiColor3D m_defaultColor;
	void Load(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene, Matrix4x4 parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, Matrix4x4 transformMatrix);
	int GetTextureIndex(aiString* pStr);
	vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ConstantBuffer<VSConstantBuffer>* m_vsConstantBuffer = nullptr;
	string m_directory = "";
};