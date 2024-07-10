
#define FULL_WINTARD // ???
#include "Surface.h"
#include <algorithm>

namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <gdiplus.h>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")

//声明信息在windows.h和comdef.h两个头文件中，一般使用SDK写程序的朋友都知道要添加windows.h头文件，其实windows.h中是含有comdef.h的声明的。
//但是如果你使用的IDE太智能了（比如我用的VC2008），在stdafx.h中会创建WIN32_LEAN_AND_MEAN宏，所以会屏蔽comdef.h，这样就会导致声明缺失问题
// Surface.h需要加XYHWin.h的头文件解决

Surface::Surface(unsigned int width, unsigned int height) noexcept
	:
	m_pBuffer(std::make_unique<Color[]>(width* height)),
	m_width(width),
	m_height(height)
{
}

Surface::Surface(Surface&& source) noexcept
	:
	m_pBuffer(std::move(source.m_pBuffer)),
	m_width(source.m_width),
	m_height(source.m_height)
{
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam, bool alphaLoaded) noexcept
	:
	m_width(width),
	m_height(height),
	m_pBuffer(std::move(pBufferParam)),
	m_alphaLoaded(alphaLoaded)
{
}

Surface& Surface::operator=(Surface&& donor) noexcept
{
	m_width = donor.m_width;
	m_height = donor.m_height;
	m_pBuffer = std::move(donor.m_pBuffer);
	donor.m_pBuffer = nullptr;

	return *this;
}

Surface::~Surface()
{
}

void Surface::Clear(Color fillValue) noexcept
{
	memset(m_pBuffer.get(), fillValue.m_dWord, m_width * m_height * sizeof(Color));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color color)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < m_width);
	assert(y < m_height);
	m_pBuffer[y * m_width + x] = color;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < m_width);
	assert(y < m_height);
	return m_pBuffer[y * m_width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return m_width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return m_height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return m_pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return m_pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return m_pBuffer.get();
}

Surface Surface::FromFile(const std::string& name)
{
	unsigned int width = 0u;
	unsigned int height = 0u;
	std::unique_ptr<Color[]> pBuffer;
	bool alphaLoaded = false;
	{
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, name.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(wideName);
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << name << "]: failed to load.";
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height); //指针数组

		for (unsigned int y = 0; y < height; ++y)
		{
			for (unsigned int x = 0; x < width; ++x)
			{
				Gdiplus::Color color;
				bitmap.GetPixel(x, y, &color);
				auto colorValue = color.GetValue();
				pBuffer[y * width + x] = color.GetValue();
				if (color.GetAlpha() != 255)
				{
					alphaLoaded = true;
				}
			}
		}
	}

	return Surface(width, height, std::move(pBuffer), alphaLoaded);
}

void Surface::Save(const std::string& filename) const
{
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
		{
			UINT num = 0; // number of image encoders
			UINT size = 0; // size of the image encoder array in bytes

			Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

			Gdiplus::GetImageEncodersSize(&num, &size);
			if (0 == size)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
				// exception
			}

			pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
			if (pImageCodecInfo == nullptr)
			{
				std::stringstream ss;
				ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			}

			GetImageEncoders(num, size, pImageCodecInfo);

			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
				{
					*pClsid = pImageCodecInfo[j].Clsid;
					free(pImageCodecInfo);
					return;
				}
			}

			free(pImageCodecInfo);
			std::stringstream ss;
			ss << "Saving surface to [" << filename <<
				"]: failed to get encoder; failed to find matching encoder.";
		};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);

	// convert filenam to wide string (for Gdiplus)
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(m_width, m_height, m_width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)m_pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
	}
}

void Surface::Copy(const Surface& src)
{
	assert(m_width == src.m_width);
	assert(m_height == src.m_height);
	memcpy(m_pBuffer.get(), src.m_pBuffer.get(), m_height * m_width * sizeof(Color));
}

bool Surface::AlphaLoaded() const noexcept
{
	return m_alphaLoaded;
}
