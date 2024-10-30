#pragma once
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>

#define RESOLVE_BASE(elementType) \
virtual size_t Resolve ## elementType() const noexcept \
{ \
	assert(false); \
	return 0; \
}


namespace DynamicData
{
	// 单个元素布局
	class LayoutElement
	{
	public:
		LayoutElement(size_t offset)
			:
			m_offset(offset)
		{
		}
		virtual LayoutElement& operator[](const char* key)
		{
			assert(false);
			return *this;
		}
		virtual const LayoutElement& operator[](const char* key) const
		{
			assert(false);
			return *this;
		}
		size_t GetOffsetBegin() const noexcept
		{
			return m_offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;

		RESOLVE_BASE(Float3);

	private:
		size_t m_offset;
	};

	class Float3 : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		size_t ResolveFloat3() const noexcept override
		{
			return GetOffsetBegin();
		}

		size_t GetOffsetEnd() const noexcept override
		{
			return GetOffsetBegin() + sizeof(DirectX::XMFLOAT3);
		}
	};

	// 结构体布局
	class Struct : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement; // 使用基类的构造函数
		LayoutElement& operator[](const char* key) override final
		{
			return *m_map.at(key);
		}

		const LayoutElement& operator[](const char* key) const override final
		{
			return *m_map.at(key);
		}

		size_t GetOffsetEnd() const noexcept override final
		{
			return m_elements.empty() ? GetOffsetBegin() : m_elements.back()->GetOffsetEnd();
		}

		// 添加元素布局
		template<typename T>
		void Add(const std::string& name)
		{
			m_elements.emplace_back(std::make_unique<T>(GetOffsetEnd()));
			if (!m_map.emplace(name, m_elements.back().get()).second)
			{
				assert(false);
			}
		}

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // 成员名对应的布局
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // 成员布局
	};

	class ElementRef
	{
	public:
		ElementRef(const LayoutElement* pLayout, char* pBytes)
			:
			m_pLayout(pLayout),
			m_pBytes(pBytes)
		{
		}

		ElementRef operator[](const char* key) noexcept
		{
			return { &(*m_pLayout)[key], m_pBytes };
		}

		// 类型转换运算符重写
		operator DirectX::XMFLOAT3& () noexcept
		{
			return *reinterpret_cast<DirectX::XMFLOAT3*>(m_pBytes + m_pLayout->ResolveFloat3());
		}

		// 赋值运算符重写
		DirectX::XMFLOAT3& operator=(const DirectX::XMFLOAT3& rhs) noexcept
		{
			// 根据布局拿到对应的Bytes数据的引用
			auto& ref = *reinterpret_cast<DirectX::XMFLOAT3*>(m_pBytes + m_pLayout->GetOffsetBegin());
			ref = rhs;
			return ref;
		}

	private:
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
	};

	class Buffer
	{
	public:
		Buffer(const Struct& layout)
			:
			m_pLayout(&layout),
			m_bytes(layout.GetOffsetEnd()) // 根据布局初始化缓存大小
		{
		}
		ElementRef operator[](const char* key)
		{
			return { &(*m_pLayout)[key], m_bytes.data() };
		}

	private:
		const class LayoutElement* m_pLayout; // 常数缓存的布局
		std::vector<char> m_bytes; // 常数缓存的数据
	};
}
