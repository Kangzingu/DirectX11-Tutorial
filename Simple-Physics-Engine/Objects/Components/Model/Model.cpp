#include "Model.h"
#include "../../Object.h"

void Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor)
{
	m_device = device;
	m_deviceContext = deviceContext;
	m_vsConstantBuffer = &vsConstantBuffer;
	m_defaultColor = defaultColor;
	Load(filePath);
}
void Model::Draw(Matrix4x4 worldMatrix, Matrix4x4& viewProjectionMatrix)
{
	m_deviceContext->VSSetConstantBuffers(0, 1, m_vsConstantBuffer->GetAddressOf());
	for (int i = 0; i < m_meshes.size(); i++)
	{
		m_vsConstantBuffer->data.wvpMatrix =  viewProjectionMatrix * (worldMatrix * m_meshes[i].m_worldMatrix);
		m_vsConstantBuffer->data.worldMatrix = worldMatrix * m_meshes[i].m_worldMatrix;
		m_vsConstantBuffer->ApplyChanges();
		m_meshes[i].Draw();
	}
}
void Model::Load(const std::string& filePath)
{
	m_directory = StringHelper::GetDirectoryFromPath(filePath);
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	ERROR_IF(pScene == nullptr, filePath + " 경로의 모델을 불러오지 못했습니다");
	// 일단 인자를 넘겨야 하긴 하니까 Identity Matrix를 넘긴듯 이거 필요한거 맞는지 확인 해보고 없앨 수 있음 없애자
	ProcessNode(pScene->mRootNode, pScene, Matrix4x4::Identity());
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, Matrix4x4 parentTransformMatrix)
{
	// ai 행렬을 directx 행렬로 바꿔주는 과정. a1은 행렬의 첫 원소(주소를 넘기는 느낌인듯)이고 DirectX는 row-major이기 때문에 transpose 해줘야 한다고 함
	Matrix4x4 nodeTransformMatrix = parentTransformMatrix * Matrix4x4(DirectX::XMMATRIX(&node->mTransformation.a1));
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene, nodeTransformMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Matrix4x4 transformMatrix)
{
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	return Mesh(m_device, m_deviceContext, vertices, indices, textures, transformMatrix);
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();

	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}
	return TextureStorageType::None;
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	// 텍스쳐가 없는 경우
	if (textureCount == 0)
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor=m_defaultColor;
		switch (textureType)
		{
			case aiTextureType_DIFFUSE:
				//pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())
				{
					materialTextures.push_back(Texture(m_device, Color::GetUnloadedTextureColor(), textureType));
					return materialTextures;
				}
				materialTextures.push_back(Texture(m_device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
				return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
				case TextureStorageType::EmbeddedIndexCompressed:
				{
					int index = GetTextureIndex(&path);
					Texture embeddedIndexedTexture(m_device, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, textureType);
					materialTextures.push_back(embeddedIndexedTexture);
					break;
				}
				case TextureStorageType::EmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
					Texture embeddedTexture(m_device, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType);
					materialTextures.push_back(embeddedTexture);
					break;
				}
				case TextureStorageType::Disk:
				{
					std::string filename = m_directory + '\\' + path.C_Str();
					Texture diskTexture(m_device, filename, textureType);
					materialTextures.push_back(diskTexture);
					break;
				}
			}
		}
	}
	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(m_device, Color::GetUnhandledTextureColor(), aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;
}

int Model::GetTextureIndex(aiString* pStr)
{
	assert(pStr->length >= 2);
	// *0 이런식으로 대있어서 숫자만 가져오기 위함
	return atoi(&pStr->C_Str()[1]);
}