#define FULL_WINTARD // ???

#include "Surface.h"
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

//声明信息在windows.h和comdef.h两个头文件中，一般使用SDK写程序的朋友都知道要添加windows.h头文件，其实windows.h中是含有comdef.h的声明的。
//但是如果你使用的IDE太智能了（比如我用的VC2008），在stdafx.h中会创建WIN32_LEAN_AND_MEAN宏，所以会屏蔽comdef.h，这样就会导致声明缺失问题
// Surface.h需要加XYHWin.h的头文件解决

Surface::Surface(unsigned int width, unsigned int height)
{
	HRESULT hr = m_scratch.Initialize2D(s_format, width, height, 1u, 1u);
	if (FAILED(hr))
	{
	}
}

Surface::Surface(DirectX::ScratchImage scratch) noexcept
	:
	m_scratch(std::move(scratch))
{
}

void Surface::Clear(Color fillValue) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imageData = *m_scratch.GetImage(0, 0, 0);

	for (size_t y = 0u; y < height; ++y)
	{
		auto rowStart = reinterpret_cast<Color*>(imageData.pixels + imageData.rowPitch * y);
		std::fill(rowStart, rowStart + imageData.width, fillValue);
	}
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color color)
{
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imageData = *m_scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&imageData.pixels[y * imageData.rowPitch])[x] = color;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept
{
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imageData = *m_scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&imageData.pixels[y * imageData.rowPitch])[x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return (unsigned int)m_scratch.GetMetadata().width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return (unsigned int)m_scratch.GetMetadata().height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>(m_scratch.GetPixels());
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>(this)->GetBufferPtr();
}

Surface Surface::FromFile(const std::string& name)
{
	bool alphaLoaded = false;

	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(wideName, DirectX::WIC_FLAGS_NONE, nullptr, scratch);

	if (FAILED(hr))
	{
	}

	// 图片格式不对
	if (scratch.GetImage(0, 0, 0)->format != s_format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),
			s_format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted);

		if (FAILED(hr))
		{
		}
		return Surface(std::move(converted));
	}

	return Surface(std::move(scratch));
}

void Surface::Save(const std::string& filename) const
{
	const auto CodecID = [](const std::string& filename)
		{
			const std::filesystem::path path = filename;
			const auto ext = path.extension().string(); // 获取文件扩展名
			if (ext == ".png")
			{
				return DirectX::WIC_CODEC_PNG;
			}
			else if (ext == ".jpg")
			{
				return DirectX::WIC_CODEC_JPEG;
			}
			else if (ext == ".bmp")
			{
				return DirectX::WIC_CODEC_BMP;
			}
		};

	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	HRESULT hr = DirectX::SaveToWICFile(
		*m_scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec(CodecID(filename)),
		wideName
	);

	if (FAILED(hr))
	{
	}
}

bool Surface::AlphaLoaded() const noexcept
{
	return !m_scratch.IsAlphaAllOpaque(); // 是全不透明
}
