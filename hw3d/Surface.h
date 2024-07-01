#pragma once

#include "XYHWin.h"
#include <string>
#include <memory>
#include <assert.h>

// ����ͼƬ����������
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
			m_dWord((r << 16u) | (g << 8u) | b)
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

		void SetX(unsigned char x) noexcept // ��m_dWord��x����ֵ��0���ٽ��µ�xֵ���ȥ
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
		unsigned int m_dWord; // ��ɫ��unsigned int��ÿһ���ֽ�0-255���ֱ��ʾ a r g b
	};

public:
	// �쳣

public:
	Surface(unsigned int width, unsigned int height) noexcept;
	Surface(Surface&& source) noexcept;
	Surface(Surface&) = delete;
	Surface& operator=(Surface&& donor) noexcept;
	Surface& operator=(const Surface&) = delete;
	~Surface();

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
	void Copy(const Surface& src);

private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept;

private:
	std::unique_ptr<Color[]> m_pBuffer;
	unsigned int m_width;
	unsigned int m_height;

};