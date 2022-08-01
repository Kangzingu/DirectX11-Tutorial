#include "Object.h"
#include "Model.h"

void Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<VSConstantBuffer>& vsConstantBuffer, aiColor3D defaultColor)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->vsConstantBuffer = &vsConstantBuffer;
	this->defaultColor = defaultColor;
	Load(filePath);
}
void Model::Draw(Matrix4x4 worldMatrix, Matrix4x4& viewProjectionMatrix)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, this->vsConstantBuffer->GetAddressOf());
	for (int i = 0; i < meshes.size(); i++)
	{
		this->vsConstantBuffer->data.wvpMatrix =  (viewProjectionMatrix * (worldMatrix * meshes[i].worldMatrix)).ToXMMATRIX();
		this->vsConstantBuffer->data.worldMatrix = (worldMatrix * meshes[i].worldMatrix).ToXMMATRIX();
		this->vsConstantBuffer->ApplyChanges();
		meshes[i].Draw();
	}
}
void Model::Load(const std::string& filePath)
{
	this->directory = StringHelper::GetDirectoryFromPath(filePath);
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	ERROR_IF(pScene == nullptr, filePath + " 경로의 모델을 불러오지 못했습니다");
	// 일단 인자를 넘겨야 하긴 하니까 Identity Matrix를 넘긴듯 이거 필요한거 맞는지 확인 해보고 없앨 수 있음 없애자
	this->ProcessNode(pScene->mRootNode, pScene, Matrix4x4::Identity());
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, Matrix4x4 parentTransformMatrix)
{
	// ai 행렬을 directx 행렬로 바꿔주는 과정. a1은 행렬의 첫 원소(주소를 넘기는 느낌인듯)이고 DirectX는 row-major이기 때문에 transpose 해줘야 한다고 함
	Matrix4x4 nodeTransformMatrix = parentTransformMatrix * Matrix4x4(DirectX::XMMATRIX(&node->mTransformation.a1));
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, Matrix4x4 transformMatrix)
{
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

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

	return Mesh(this->device, this->deviceContext, vertices, indices, textures, transformMatrix);
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
		aiColor3D aiColor=this->defaultColor;
		switch (textureType)
		{
			case aiTextureType_DIFFUSE:
				//pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
				if (aiColor.IsBlack())
				{
					materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
					return materialTextures;
				}
				materialTextures.push_back(Texture(this->device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
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
					Texture embeddedIndexedTexture(this->device, reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData), pScene->mTextures[index]->mWidth, textureType);
					materialTextures.push_back(embeddedIndexedTexture);
					break;
				}
				case TextureStorageType::EmbeddedCompressed:
				{
					const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
					Texture embeddedTexture(this->device, reinterpret_cast<uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType);
					materialTextures.push_back(embeddedTexture);
					break;
				}
				case TextureStorageType::Disk:
				{
					std::string filename = this->directory + '\\' + path.C_Str();
					Texture diskTexture(this->device, filename, textureType);
					materialTextures.push_back(diskTexture);
					break;
				}
			}
		}
	}
	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;
}

int Model::GetTextureIndex(aiString* pStr)
{
	assert(pStr->length >= 2);
	// *0 이런식으로 대있어서 숫자만 가져오기 위함
	return atoi(&pStr->C_Str()[1]);
}