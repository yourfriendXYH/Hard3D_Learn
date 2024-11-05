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

// 布局元素宏
#define LEAF_ELEMENT(elementType, sysType) \
class elementType : public LayoutElement \
{ \
public: \
	using SystemType = sysType; \
	using LayoutElement::LayoutElement; \
	size_t Resolve ## elementType() const noexcept override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + sizeof(sysType); \
	} \
};

// 类型转换运算符 和 赋值运算符 的重写
#define REF_CONVERSION(elementType) \
operator elementType::SystemType& () noexcept \
{ \
	return *reinterpret_cast<elementType::SystemType*>(m_pBytes + m_pLayout->Resolve ## elementType()); \
} \
elementType::SystemType& operator=(const elementType::SystemType& rhs) noexcept \
{ \
	return static_cast<elementType::SystemType&>(*this) = rhs; \
}

namespace DynamicData
{
	class Struct;
	class Array;
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

		// Array需要使用，用于返回自身
		virtual LayoutElement& LayoutEle()
		{
			assert(false);
			return *this;
		}
		virtual const LayoutElement& LayoutEle() const
		{
			assert(false);
			return *this;
		}

		size_t GetOffsetBegin() const noexcept
		{
			return m_offset;
		}
		virtual size_t GetOffsetEnd() const noexcept = 0;

		// 获取布局元素的大小
		size_t GetSizeInBytes() const noexcept
		{
			return GetOffsetEnd() - GetOffsetBegin();
		}

		template<typename T>
		Struct& Add(const std::string& key) noexcept;

		template<typename T>
		Array& Set(size_t size_in) noexcept;

		RESOLVE_BASE(Matrix);
		RESOLVE_BASE(Float4);
		RESOLVE_BASE(Float3);
		RESOLVE_BASE(Float2);
		RESOLVE_BASE(Float);
		RESOLVE_BASE(Bool);

	private:
		size_t m_offset;
	};

	LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
	LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
	LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
	LEAF_ELEMENT(Float, float);
	LEAF_ELEMENT(Bool, bool);

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
		Struct& Add(const std::string& name)
		{
			m_elements.emplace_back(std::make_unique<T>(GetOffsetEnd()));
			if (!m_map.emplace(name, m_elements.back().get()).second)
			{
				assert(false);
			}
			return *this;
		}

	private:
		std::unordered_map<std::string, LayoutElement*> m_map; // 成员名对应的布局
		std::vector<std::unique_ptr<LayoutElement>> m_elements; // 成员布局
	};

	class Array : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;
		size_t GetOffsetEnd() const noexcept override
		{
			assert(nullptr != m_upElement);
			return GetOffsetBegin() + m_upElement->GetSizeInBytes() * m_size;
		}

		template<typename T>
		Array& Set(size_t size_in) noexcept
		{
			assert(nullptr == m_upElement);
			m_upElement = std::make_unique<T>(GetOffsetBegin());
			m_size = size_in;
			return *this;
		}

		// 拿到数组的元素布局
		LayoutElement& LayoutEle() override final
		{
			return *m_upElement;
		}
		const LayoutElement& LayoutEle() const override final
		{
			return *m_upElement;
		}

	private:
		size_t m_size = 0u; // 数组大小
		std::unique_ptr<LayoutElement> m_upElement; // 单个元素的布局
	};

	template<typename T>
	Struct& LayoutElement::Add(const std::string& key) noexcept
	{
		auto pStruct = dynamic_cast<Struct*>(this);
		assert(nullptr != pStruct);
		return pStruct->Add<T>(key);
	}

	template<typename T>
	Array& LayoutElement::Set(size_t size_in) noexcept
	{
		auto pArray = dynamic_cast<Array*>(this);
		assert(nullptr != pArray);
		return pArray->Set<T>(size_in);
	}

	class ElementRef
	{
	public:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset)
			:
			m_pLayout(pLayout),
			m_pBytes(pBytes),
			m_offset(offset)
		{
		}

		ElementRef operator[](const char* key) noexcept
		{
			return { &(*m_pLayout)[key], m_pBytes, m_offset };
		}

		ElementRef operator[](size_t index) noexcept
		{
			const auto& t = m_pLayout->LayoutEle();
			return { &t, m_pBytes, m_offset + t.GetSizeInBytes() * index };
		}

		//// 类型转换运算符重写
		//operator DirectX::XMFLOAT3& () noexcept
		//{
		//	return *reinterpret_cast<DirectX::XMFLOAT3*>(m_pBytes + m_pLayout->ResolveFloat3());
		//}

		//// 赋值运算符重写
		//DirectX::XMFLOAT3& operator=(const DirectX::XMFLOAT3& rhs) noexcept
		//{
		//	// 根据布局拿到对应的Bytes数据的引用
		//	auto& ref = *reinterpret_cast<DirectX::XMFLOAT3*>(m_pBytes + m_pLayout->GetOffsetBegin());
		//	ref = rhs;
		//	return ref;
		//}

		REF_CONVERSION(Matrix);
		REF_CONVERSION(Float4);
		REF_CONVERSION(Float3);
		REF_CONVERSION(Float2);
		REF_CONVERSION(Float);
		REF_CONVERSION(Bool);

	private:
		const class LayoutElement* m_pLayout;
		char* m_pBytes;
		size_t m_offset;
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
			return { &(*m_pLayout)[key], m_bytes.data(), 0u };
		}

	private:
		const class LayoutElement* m_pLayout; // 常数缓存的布局
		std::vector<char> m_bytes; // 常数缓存的数据
	};
}
