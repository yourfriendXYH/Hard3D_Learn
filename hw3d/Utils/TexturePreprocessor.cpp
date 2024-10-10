#include "TexturePreprocessor.h"
#include <filesystem>
#include "../Drawable/Mesh.h"

void TexturePreprocessor::FlipYAllNormalMapsInObj(const std::string& objPath)
{
	const auto rootPath = std::filesystem::path{ objPath }.parent_path().string() + "\\";

	// ��ȡ��������
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(objPath.c_str(), 0u);
	if (nullptr == pScene)
	{
		// ��ȡʧ��
	}

	using namespace DirectX;
	const auto flipY = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	// ����Yֵ����
	const auto ProcessNormal = [flipY](FXMVECTOR normal)->XMVECTOR
		{
			return XMVectorMax(normal, flipY);
		};

	// ѭ���������е�normal texture
	for (auto i = 0u; i < pScene->mNumMaterials; ++i)
	{
		const auto& material = *pScene->mMaterials[i];
		aiString textureFileName; // texture���ļ���
		if (material.GetTexture(aiTextureType_NORMALS, 0u, &textureFileName) == aiReturn_SUCCESS)
		{
			const auto normalTexturePath = rootPath + textureFileName.C_Str();
			// ����������
			TransformFile(normalTexturePath, normalTexturePath, ProcessNormal);
		}
	}
}

template<typename F>
inline void TexturePreprocessor::TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func)
{
	auto surface = Surface::FromFile(pathIn);

	const auto nPixels = surface.GetWidth() * surface.GetHeight(); // �������ݵ�����
	const auto pBegin = surface.GetBufferPtr();
	const auto pEnd = surface.GetBufferPtrConst() + nPixels;

	for (auto pCurrent = pBegin; pCurrent < pEnd; ++pCurrent)
	{
		const auto normalVec = ColorToVector(*pCurrent);
		*pCurrent = VectorToColor(func(normalVec)); // ��ת����ķ��߷����ٸ���texture
	}

	surface.Save(pathOut);
}

DirectX::XMVECTOR TexturePreprocessor::ColorToVector(Surface::Color color) noexcept
{
	using namespace DirectX;
	auto n = XMVectorSet((float)color.GetR(), (float)color.GetG(), (float)color.GetB(), 0.0f);
	const auto all255 = XMVectorReplicate(2.0f / 255.0f);
	n = XMVectorMultiply(n, all255);
	const auto all1 = XMVectorReplicate(1.0f);
	n = XMVectorSubtract(n, all1);
	return n;
}

Surface::Color TexturePreprocessor::VectorToColor(DirectX::FXMVECTOR normal) noexcept
{
	using namespace DirectX;
	const auto all1 = XMVectorReplicate(1.0f);
	XMVECTOR nOut = XMVectorAdd(normal, all1);
	const auto all255 = XMVectorReplicate(255.0f / 2.0f);
	nOut = XMVectorMultiply(nOut, all255);
	XMFLOAT3 floats;
	XMStoreFloat3(&floats, nOut);
	return {
		(unsigned char)round(floats.x),
		(unsigned char)round(floats.y),
		(unsigned char)round(floats.z),
	};
}
