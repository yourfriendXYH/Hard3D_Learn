#include "TexturePreprocessor.h"
#include <filesystem>
#include "../Drawable/Mesh.h"

void TexturePreprocessor::FlipYAllNormalMapsInObj(const std::string& objPath)
{
	const auto rootPath = std::filesystem::path{ objPath }.parent_path().string() + "\\";

	// 读取纹理数据
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(objPath.c_str(), 0u);
	if (nullptr == pScene)
	{
		// 读取失败
	}

	// 循环处理所有的normal texture
	for (auto i = 0u; i < pScene->mNumMaterials; ++i)
	{
		const auto& material = *pScene->mMaterials[i];
		aiString textureFileName; // texture的文件名
		if (material.GetTexture(aiTextureType_NORMALS, 0u, &textureFileName) == aiReturn_SUCCESS)
		{
			const auto normalTexturePath = rootPath + textureFileName.C_Str();
			// 处理法线数据
			FlipYNormalMap(objPath, objPath);
		}
	}
}

void TexturePreprocessor::FlipYNormalMap(const std::string& pathIn, const std::string& pathOut)
{
	using namespace DirectX;
	const auto flipY = XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	// 向量Y值反向
	const auto ProcessNormal = [flipY](FXMVECTOR normal, int x, int y) -> XMVECTOR
		{
			return XMVectorMax(normal, flipY);
		};

	TransformFile(pathIn, pathOut, ProcessNormal);
}

void TexturePreprocessor::ValidateNormalMap(const std::string& pathIn, float thresholdMin, float thresholdMax)
{
	OutputDebugStringA(("Validating normal map [" + pathIn + "]\n").c_str());

	using namespace DirectX;
	auto sum = XMVectorZero();

	// 验证法线贴图的函数
	// 如果出错则指出哪个像素的法线出错
	const auto ProcessNormal = [thresholdMin, thresholdMax, &sum](FXMVECTOR normal, int x, int y)->XMVECTOR
		{
			const auto len = XMVectorGetX(XMVector3Length(normal)); // 法线长度
			const auto z = XMVectorGetZ(normal);
			// 法线长度出错
			if (len < thresholdMin || len > thresholdMax)
			{
				XMFLOAT3 vec;
				XMStoreFloat3(&vec, normal);
				std::ostringstream oss;

				oss << "Bad normal length: " << len << " at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
				OutputDebugStringA(oss.str().c_str());
			}
			if (z < 0.0f) // 法线的z值方向出错
			{
				XMFLOAT3 vec;
				XMStoreFloat3(&vec, normal);
				std::ostringstream oss;
				oss << "Bad normal Z direction at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
				OutputDebugStringA(oss.str().c_str());
			}
			sum = XMVectorAdd(sum, normal);
			return normal;
		};

	auto surface = Surface::FromFile(pathIn);
	TransformSurface(surface, ProcessNormal);

	// 输出处理后的法线贴图的信息
	XMFLOAT2 sumv;
	XMStoreFloat2(&sumv, sum);
	std::ostringstream oss;
	// 法线贴图偏差？？？
	oss << "Normal map biases: (" << sumv.x << "," << sumv.y << ")\n";
	OutputDebugStringA(oss.str().c_str());
}

void TexturePreprocessor::MakeStripes(const std::string& pathOut, int size, int stripeWidth)
{
	// make sure texture dimension is power of 2
	auto power = log2(size);
	assert(modf(power, &power) == 0.0);
	// make sure stripe width enables at least 2 stripes
	assert(stripeWidth < size / 2);

	Surface s(size, size);
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			Surface::Color c = { 0,0,0 };
			if ((x / stripeWidth) % 2 == 0)
			{
				c = { 255,255,255 };
			}
			s.PutPixel(x, y, c);
		}
	}
	s.Save(pathOut);
}

template<typename F>
inline void TexturePreprocessor::TransformFile(const std::string& pathIn, const std::string& pathOut, F&& func)
{
	auto surface = Surface::FromFile(pathIn);

	//const auto nPixels = surface.GetWidth() * surface.GetHeight(); // 像素数据的数量
	//const auto pBegin = surface.GetBufferPtr();
	//const auto pEnd = surface.GetBufferPtrConst() + nPixels;

	//for (auto pCurrent = pBegin; pCurrent < pEnd; ++pCurrent)
	//{
	//	const auto normalVec = ColorToVector(*pCurrent);
	//	*pCurrent = VectorToColor(func(normalVec)); // 将转换后的法线方向，再给到texture
	//}

	TransformSurface(surface, func);

	surface.Save(pathOut);
}

template<typename F>
inline void TexturePreprocessor::TransformSurface(Surface& surface, F&& func)
{
	// 纹理的像素长宽
	const auto pixelWidth = surface.GetWidth();
	const auto pixelHeight = surface.GetHeight();
	for (unsigned int y = 0; y < pixelHeight; ++y)
	{
		for (unsigned int x = 0; x < pixelWidth; ++x)
		{
			const auto normalVec = ColorToVector(surface.GetPixel(x, y)); // 获取指定像素点
			surface.PutPixel(x, y, VectorToColor(func(normalVec, x, y)));
		}
	}

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
