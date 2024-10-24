#pragma once

#include "XYHWin.h"
#include <string>
#include <memory>
#include <assert.h>
#include "dxtex/include/dxtex/DirectXTex.h"

// 处理图片的像素数据
class Surface
{
public:
	class Color
	{
	public:
		constexpr Color() noexcept : m_dWord()
		{}
		constexpr Color(const Color& color) noexcept
			:
			m_dWord(color.m_dWord)
		{}
		constexpr Color(unsigned int dWord) noexcept
			:
			m_dWord(dWord)
		{}
		constexpr Color(unsigned int x, unsigned int r, unsigned int g, unsigned int b) noexcept
			:
			m_dWord((x << 24u) | (r << 16u) | (g << 8u) | b)
		{}
		constexpr Color(unsigned int r, unsigned int g, unsigned int b) noexcept
			:
			m_dWord((255u << 24) | (r << 16u) | (g << 8u) | b)
		{}
		constexpr Color(const Color& color, unsigned int x) noexcept
			:
			Color((x << 24u) | color.m_dWord)
		{}

		Color& operator=(Color color) noexcept
		{
			m_dWord = color.m_dWord;
			return *this;
		}

		constexpr unsigned char GetX() const noexcept
		{
			return m_dWord >> 24u;
		}
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept
		{
			return (m_dWord >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept
		{
			return (m_dWord >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept
		{
			return m_dWord & 0xFFu;
		}

		void SetX(unsigned char x) noexcept // 将m_dWord的x处的值赋0，再将新的x值或进去
		{
			m_dWord = (m_dWord & 0xFFFFFFu) | (x << 24u);
		}

		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}

		void SetR(unsigned char r) noexcept
		{
			m_dWord = (m_dWord & 0xFF00FFFF) | (r << 16u);
		}

		void SetG(unsigned char g) noexcept
		{
			m_dWord = (m_dWord & 0xFFFF00FF) | (g << 8u);
		}

		void SetB(unsigned char b) noexcept
		{
			m_dWord = (m_dWord & 0xFFFFFF00) | b;
		}

	public:
		unsigned int m_dWord; // 颜色用unsigned int，每一个字节0-255，分别表示 a r g b
	};

public:
	// 异常

public:
	Surface(unsigned int width, unsigned int height);
	Surface(Surface&& source) noexcept = default;
	Surface(Surface&) = delete;
	Surface& operator=(Surface&& donor) noexcept = default;
	Surface& operator=(const Surface&) = delete;
	~Surface() = default;

	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color color);
	Color GetPixel(unsigned int x, unsigned int y) const noexcept;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;

	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;

	static Surface FromFile(const std::string& name);
	void Save(const std::string& filename) const;

	bool AlphaLoaded() const noexcept;

private:
	Surface(DirectX::ScratchImage scratch) noexcept;

private:
	static constexpr DXGI_FORMAT s_format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;

	DirectX::ScratchImage m_scratch;
};